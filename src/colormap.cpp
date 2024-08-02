#include "colormap.h"
#include "PlotUtils.h"

//Compile time constants
constexpr int OFFSET_COLORMAP_COLORBAR = 8;
constexpr int COLORBAR_WIDTH = 10;
constexpr int COLORBAR_AXIS_TEXT_WIDTH = 80;
constexpr int PADDING_TITLE_COLORMAP = 10;
constexpr int PADDING_COLORMAP_XAXIS = 30;
constexpr int COLORMAP_BORDER_THICKNESS = 1;
constexpr int DEFAULT_NUMBER_OF_COLORBAR_AXES = 6;
constexpr int MINIMUM_COLORBAR_AXIS_DISTANCE = 30;
constexpr int COLORMAP_BORDER_LENGTH = (2 * COLORMAP_BORDER_THICKNESS);


//This namespace should be dominant for the scope of this file
using namespace PainterConstants;

namespace{
    cv::Mat getColorbar(const int colorbarHeight, const cv::ColormapTypes colormapType)
    {
        //Generate a colorbar with fixed size
        cv::Mat colorbar(255, COLORBAR_WIDTH, CV_8U);
        const auto lambda_generateGradient = [](uint8_t& pixel_value, const int* pos){ pixel_value = 255 - pos[0]; };
        colorbar.forEach<uint8_t>(lambda_generateGradient);

        //Resize the colorbar and apply the colormap
        cv::resize(colorbar, colorbar, cv::Size{COLORBAR_WIDTH, colorbarHeight},0, 0, cv::InterpolationFlags::INTER_NEAREST);
        cv::applyColorMap(colorbar, colorbar, colormapType);
        return colorbar;
    }
}

Colormap::Colormap(const cv::Mat &target, const cv::ColormapTypes colormapType) : m_colormapType(colormapType)
{
    cv::normalize(target, m_colormap, 0, 255, cv::NormTypes::NORM_MINMAX);
    cv::applyColorMap(m_colormap, m_colormap, colormapType);

    double targetMin, targetMax;
    cv::minMaxLoc(target, &targetMin, &targetMax);
    m_colormapRange = {targetMin, targetMax};
}


Colormap::Colormap(const cv::Mat& target,
                   const std::optional<double> t_colormap_min,
                   const std::optional<double> t_colormap_max,
                   const cv::ColormapTypes colormapType): m_colormapType(colormapType)
{
    //Get the minimum and maximum value in an array
    double targetMin, targetMax;
    cv::minMaxLoc(target, &targetMin, &targetMax);

    //Deduce the colormap range and assign it to the member
    const double colormap_min = (t_colormap_min)? *t_colormap_min : targetMin;
    const double colormap_max = (t_colormap_max)? *t_colormap_max : targetMax;
    m_colormapRange = {colormap_min, colormap_max};

    //Check if the maximum colormap bound is larger
    if(colormap_min > colormap_max)
        throw std::runtime_error("Minimum colormap bound should not be larger than the maximum bound");

    //Check if any elements are in bounds
    if((targetMin > colormap_max) || (targetMax < colormap_min))
        throw std::runtime_error("At least one element should be inside of the colormap bounds");

    //Truncate the pixels that are outside of the lower and upper bounds of the colormap boundaries
    cv::Mat thresholded;
    cv::threshold(target, thresholded, colormap_max, colormap_max, cv::ThresholdTypes::THRESH_TRUNC);
    thresholded.setTo(colormap_min, thresholded < colormap_min);

    //Normalize the target to the range of the colormap
    cv::Mat normalized;
    cv::normalize(thresholded, normalized, 0, 255, cv::NormTypes::NORM_MINMAX, CV_8U);

    //Apply the colormap
    cv::applyColorMap(normalized, m_colormap, colormapType);
}

cv::Mat Colormap::generate()
{
    if(m_colormap.empty())
        throw std::runtime_error("The colormap target cannot be empty");

    //Generate the title and x-Axis text but don't place it on the canvas yet. Size of these canvases will determine the size of the main canvas
    cv::Mat xAxisCanvas = generateText(m_xAxisSize, m_xAxisText, m_xAxisColor);
    cv::Mat titleCanvas = generateText(m_titleSize, m_title, m_titleColor);

    //There is a lower limit on the sizes that a canvas can have
    const cv::Size minimumCanvasSize = calculateMinimumCanvasSize(titleCanvas.size(), xAxisCanvas.size());
    canvasSize.height = std::max(canvasSize.height, minimumCanvasSize.height);
    canvasSize.width = std::max(canvasSize.width, minimumCanvasSize.width);

    //Generate a blank canvas
    m_canvas = cv::Mat(canvasSize, CV_8UC3, white);

    //This counter keeps track of the last row position on the canvas
    int canvasRowCounter = 0;

    //Add top padding to the row counter
    canvasRowCounter += CANVAS_HEIGHT_PADDING;

    //Place the title canvas that has previously been generated
    centerElement(titleCanvas, cv::Size{canvasSize.width, 0}, AlignmentType::WidthOnly);
    titleCanvas.copyTo(m_canvas(cv::Rect(0, canvasRowCounter, titleCanvas.cols, titleCanvas.rows)));

    canvasRowCounter += titleCanvas.rows + PADDING_TITLE_COLORMAP;

    //Generate the colormap and place it on canvas
    cv::Mat colormapCanvas = generateColormapCanvas(titleCanvas.rows, xAxisCanvas.rows);
    const int colormapAllocatedHeight = m_canvas.rows - totalHeightPadding() - titleCanvas.rows - xAxisCanvas.rows;
    centerElement(colormapCanvas, cv::Size{canvasSize.width, colormapAllocatedHeight}, AlignmentType::WholeShape);
    colormapCanvas.copyTo(m_canvas(cv::Rect(0, canvasRowCounter, colormapCanvas.cols, colormapCanvas.rows)));

    canvasRowCounter += colormapCanvas.rows + PADDING_COLORMAP_XAXIS;

    //Place the x-axis text that previously generated
    centerElement(xAxisCanvas, cv::Size{canvasSize.width, 0}, AlignmentType::WidthOnly);
    xAxisCanvas.copyTo(m_canvas(cv::Rect(0, canvasRowCounter, xAxisCanvas.cols, xAxisCanvas.rows)));

    //Generate the histogram graph element.
    return m_canvas;
}

cv::Size Colormap::calculateMinimumCanvasSize(const cv::Size& titleCanvasSize, const cv::Size& xAxisCanvasSize)
{    
    //Determine the space required for axis number texts
    m_xAxisTextSize = allocateNumericTextSpace(DEFAULT_AXIS_NUMBER_SIZE, 1.25, m_precision_x);
    m_yAxisTextSize = allocateNumericTextSpace(DEFAULT_AXIS_NUMBER_SIZE, 1.25, m_precision_y);
    m_colorbarTextSize = allocateNumericTextSpace(DEFAULT_AXIS_NUMBER_SIZE, 1.25, m_colorbarPrecision);

    const int minimumColormapHeight = m_colormap.rows + COLORMAP_BORDER_LENGTH + m_xAxisTextSize.height;
    const int minimumColormapWidthWithColorbar = m_yAxisTextSize.width + COLORMAP_BORDER_LENGTH + m_colormap.cols + colorbarTotalWidth();

    //Combine minimum sizes
    const int totalHeight = titleCanvasSize.height + minimumColormapHeight + xAxisCanvasSize.height + totalHeightPadding();
    const int totalWidth = std::max({titleCanvasSize.width, minimumColormapWidthWithColorbar, xAxisCanvasSize.width}) + (2 * CANVAS_WIDTH_PADDING);

    return cv::Size{totalWidth, totalHeight};
}


cv::Mat Colormap::generateColormapCanvas(const int titleCanvasHeight, const int xAxisCanvasHeight) const
{
    const int canvasWidthWithoutColormap = colorbarTotalWidth() + COLORMAP_BORDER_LENGTH + yAxisTextWidth();

    //Determine the available size for colormap to place
    const auto& [canvasWidth, canvasHeight] = m_canvas.size();
    const int colormapAvailableWidth = canvasWidth - (2 * CANVAS_WIDTH_PADDING) - canvasWidthWithoutColormap;
    const int colormapAvailableHeight = canvasHeight - totalHeightPadding() - titleCanvasHeight - xAxisTextHeight() - xAxisCanvasHeight - COLORMAP_BORDER_LENGTH;

    //Resize the colormap considering the aspect ratio and the available space
    const float aspectRatio = static_cast<float>(m_colormap.cols) / m_colormap.rows;
    const float availableZoomFactor_y = static_cast<float>(colormapAvailableWidth) / m_colormap.cols;
    const float availableZoomFactor_x = static_cast<float>(colormapAvailableHeight) / m_colormap.rows;
    int colormapWidth, colormapHeight;
    if(availableZoomFactor_y >= availableZoomFactor_x){
        colormapHeight = colormapAvailableHeight;
        colormapWidth = static_cast<int>(colormapHeight * aspectRatio);
    }
    else{
        colormapWidth = colormapAvailableWidth;
        colormapHeight = static_cast<int>(colormapWidth / aspectRatio);
    }
    cv::Mat colormap_resized;
    cv::resize(m_colormap, colormap_resized, {colormapWidth, colormapHeight}, 0, 0, cv::InterpolationFlags::INTER_NEAREST);

    //Prepare the output canvas
    cv::Mat out(colormapHeight + COLORMAP_BORDER_LENGTH + xAxisTextHeight(), colormapWidth + canvasWidthWithoutColormap, CV_8UC3, white);

    //Draw a border around colormap to indicate the area
    cv::rectangle(out,
                  cv::Rect(yAxisTextWidth(), 0, colormapWidth + COLORMAP_BORDER_LENGTH, colormapHeight + COLORMAP_BORDER_LENGTH),
                  black,
                  COLORMAP_BORDER_THICKNESS,
                  cv::LINE_AA);

    //Place the colormap on the canvas
    int horizontalPos = yAxisTextWidth() + COLORMAP_BORDER_THICKNESS;
    int verticalPos = COLORMAP_BORDER_THICKNESS;
    colormap_resized.copyTo(out(cv::Rect(horizontalPos, verticalPos, colormapWidth, colormapHeight)));

    horizontalPos += colormapWidth + OFFSET_COLORMAP_COLORBAR;

    //Generate a colorbar and place the colorbar on the canvas
    cv::Mat colorbar = generateColorbar(colormapHeight + COLORMAP_BORDER_LENGTH);
    colorbar.copyTo(out(cv::Rect(horizontalPos, 0, colorbar.cols, colorbar.rows)));

    //Add axis texts. Remove colorbar area to prevent wrong element width estimation
    cv::Mat colorbar_removed = out.colRange(0, out.cols - colorbarTotalWidth());
    addAxis(colorbar_removed, 0, 0, {0, m_colormap.cols}, {0, m_colormap.rows});

    return out;
}

cv::Mat Colormap::generateColorbar(const int colormapHeight) const
{
    //Prepare the canvas
    cv::Mat out(colormapHeight, colorbarTotalWidth() - OFFSET_COLORMAP_COLORBAR, CV_8UC3, white);

    cv::Mat colorbar = getColorbar(colormapHeight, m_colormapType);
    colorbar.copyTo(out(cv::Rect(0, 0, COLORBAR_WIDTH, colormapHeight)));

    //Generate each colorbar number
    const int numberofColorbarAxes = std::min(DEFAULT_NUMBER_OF_COLORBAR_AXES, std::max(colorbar.rows / MINIMUM_COLORBAR_AXIS_DISTANCE, 1));
    const auto&[colormapMin, colormapMax] = m_colormapRange;
    const auto colorbarAxes = PlotUtils::linspace(colormapMin, colormapMax, numberofColorbarAxes);
    const auto colorbarPositions = PlotUtils::linspace(colormapHeight, 0, numberofColorbarAxes);

    //Place each colorbar number
    for(auto it_axes=colorbarAxes.cbegin(), it_pos=colorbarPositions.cbegin();it_axes != colorbarAxes.cend(); it_pos++, it_axes++){
        //Draw the axis line
        const int pos = static_cast<int>(*it_pos);
        cv::line(out, cv::Point{COLORBAR_WIDTH, pos}, cv::Point{COLORBAR_WIDTH + LENGTH_AXIS_LINE, pos}, cv::LINE_AA);

        //Draw the text
        cv::Mat textCanvas = generateNumericText(DEFAULT_AXIS_NUMBER_SIZE, *it_axes, m_colorbarPrecision);
        int yStart = std::max(pos - (textCanvas.rows / 2), 0);
        yStart = std::min(yStart, colormapHeight - textCanvas.rows);
        textCanvas.copyTo(out(cv::Rect(COLORBAR_WIDTH + LENGTH_AXIS_LINE, yStart, textCanvas.cols, textCanvas.rows)));
    }

    return out;
}

int Colormap::colorbarTotalWidth() const
{
    return OFFSET_COLORMAP_COLORBAR + COLORBAR_WIDTH + LENGTH_AXIS_LINE + m_colorbarTextSize.width;
}

int Colormap::totalHeightPadding() const
{
    return (2 * CANVAS_HEIGHT_PADDING) + PADDING_TITLE_COLORMAP + PADDING_COLORMAP_XAXIS;
}
