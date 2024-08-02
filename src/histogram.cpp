#include "histogram.h"
#include <numeric>
#include "opencv2/imgproc.hpp"
#include "PlotUtils.h"

//We will clearly use constants from this namespace
using namespace PainterConstants;

//Compile time constants
constexpr int PADDING_TITLE_HISTOGRAM = 10;
constexpr int PADDING_HISTOGRAM_XAXIS = 10;
constexpr int MINIMUM_HISTOGRAM_HEIGHT = 200;


static inline void fieldNotFoundError(const std::string& text)
{
    throw(std::runtime_error(text + "doesn't exist on histgram element"));
}

static inline void improperFieldError(const std::string&operation, const std::string& input)
{
    throw(std::runtime_error("The operation '" + operation + "' is incompitable with the field '" + input + "'"));
}


Histogram::Histogram(const std::vector<size_t> &histogram, const std::vector<double> &bins) : m_histogram(histogram), m_bins(bins)
{
    if(!m_histogram.size())
        throw(std::runtime_error("Length of the histogram cannot be zero"));

    if(m_histogram.size() != m_bins.size())
        throw(std::runtime_error("Length mismatch between histogram vector and bin vector"));
}

Histogram::Histogram(std::vector<size_t> &&histogram, std::vector<double> &&bins) : m_histogram(std::move(histogram)), m_bins(std::move(bins))
{
    if(!m_histogram.size())
        throw(std::runtime_error("Length of the histogram cannot be zero"));

    if(m_histogram.size() != m_bins.size())
        throw(std::runtime_error("Length mismatch between histogram vector and bin vector"));
}

Histogram::Histogram(const std::vector<size_t> &histogram) : m_histogram(histogram), m_bins(histogram.size())
{
    if(!m_histogram.size())
        throw(std::runtime_error("Length of the histogram cannot be zero"));

    std::iota(m_bins.begin(), m_bins.end(), 1);
}

Histogram::Histogram(std::vector<size_t> &&histogram) : m_histogram(std::move(histogram)), m_bins(m_histogram.size())
{
    if(!m_histogram.size())
        throw(std::runtime_error("Length of the histogram cannot be zero"));

    std::iota(m_bins.begin(), m_bins.end(), 1);
}

Histogram::Histogram(const std::vector<size_t> &histogram, const double binStart, const std::optional<double> binEnd) : m_histogram(histogram)
{
    if(!m_histogram.size())
        throw(std::runtime_error("Length of the histogram cannot be zero"));

    const double binEnd_ = (binEnd)? *binEnd : binStart + histogram.size();
    m_bins = PlotUtils::linspace(binStart, binEnd_, histogram.size());
}

Histogram::Histogram(std::vector<size_t> &&histogram, const double binStart, const std::optional<double> binEnd) : m_histogram(std::move(histogram))
{
    if(!m_histogram.size())
        throw(std::runtime_error("Length of the histogram cannot be zero"));

    const double binEnd_ = (binEnd)? *binEnd : binStart + m_histogram.size();
    m_bins = PlotUtils::linspace(binStart, binEnd_, m_histogram.size());
}

Histogram::Histogram(const cv::Mat &inArray, const std::optional<int> t_binSize, const std::optional<double> t_binStart, const std::optional<double> t_binEnd)
{
    if(t_binSize)
        if(*t_binSize == 0)
            throw(std::invalid_argument("number of bins cannot be zero"));

    // At least one of the range parameters isn't nullopt so auto-range will be necessary
    double minVal{}, maxVal{};
    if((!t_binStart) || (!t_binEnd)){
        cv::minMaxLoc(inArray, &minVal, &maxVal, NULL, NULL);
    }

    // This lambda expression extends the min-max value for padding
    constexpr float HISTOGRAM_AXES_PADDING_PERCENTAGE = 0.05f;
    const auto lambda_addLeftPadding = [HISTOGRAM_AXES_PADDING_PERCENTAGE](float value) {return (value > 0)? value * (1 - HISTOGRAM_AXES_PADDING_PERCENTAGE) : value * (1 + HISTOGRAM_AXES_PADDING_PERCENTAGE); };
    const auto lambda_addRightPaddng = [HISTOGRAM_AXES_PADDING_PERCENTAGE](float value) {return (value > 0)? value * (1 + HISTOGRAM_AXES_PADDING_PERCENTAGE) : value * (1 - HISTOGRAM_AXES_PADDING_PERCENTAGE); };

    // Determine the ranges
    const float binStart = static_cast<float>(t_binStart.value_or(lambda_addLeftPadding(minVal)));
    const float binEnd = static_cast<float>((t_binEnd).value_or(lambda_addRightPaddng(maxVal)));
    const int binSize = (t_binSize).value_or( binEnd - binStart + 1);

    // Apply OpenCV histogram calculation
    cv::Mat hist;
    const std::vector ranges{binStart, binEnd};
    const std::vector bins{binSize};
    std::vector<cv::Mat>vMat{inArray};
    std::vector<int> chs{0};  
    cv::calcHist(vMat, chs, cv::Mat(), hist, bins, ranges, true);

    m_histogram = std::vector<size_t>(hist.begin<float>(), hist.end<float>());
    m_bins = PlotUtils::linspace(binStart, binEnd, binSize);
}

cv::Mat Histogram::generate()
{
    if(!m_histogram.size() || !m_bins.size())
        throw(std::runtime_error("Length of the histogram or bins vector cannot be zero"));

    //Generate the title and x-axis text beforehand.
    cv::Mat titleCanvas = generateText(m_titleSize, m_title, m_titleColor);
    cv::Mat xAxisCanvas = generateText(m_xAxisSize, m_xAxisText, m_xAxisColor);

    //There is a lower limit on the sizes that a canvas can have
    const auto minimumCanvasSize = calculateMinimumCanvasSize(titleCanvas.size(), xAxisCanvas.size());
    canvasSize.height = std::max(canvasSize.height, minimumCanvasSize.height);
    canvasSize.width = std::max(canvasSize.width, minimumCanvasSize.width);

    //Generate a blank canvas
    m_canvas = cv::Mat(canvasSize, CV_8UC3, white);

    //This counter keeps track of the last row position on the canvas
    int canvasRowCounter = 0;

    //Add top padding to the row counter
    canvasRowCounter += CANVAS_HEIGHT_PADDING;

    //Reshape the previously generated title and place it on the canvas
    centerElement(titleCanvas, cv::Size{canvasSize.width, 0}, AlignmentType::WidthOnly);
    titleCanvas.copyTo(m_canvas(cv::Rect(0, canvasRowCounter, titleCanvas.cols, titleCanvas.rows)));

    canvasRowCounter += titleCanvas.rows + PADDING_TITLE_HISTOGRAM;

    //Generate the histogram and place it on canvas
    cv::Mat histogramCanvas = generateHistogramCanvas(titleCanvas.rows, xAxisCanvas.rows);
    centerElement(histogramCanvas, cv::Size{canvasSize.width, 0}, AlignmentType::WidthOnly);
    histogramCanvas.copyTo(m_canvas(cv::Rect(0, canvasRowCounter, histogramCanvas.cols, histogramCanvas.rows)));

    canvasRowCounter += histogramCanvas.rows + PADDING_HISTOGRAM_XAXIS;

    //Place the x-axis text that previously generated
    centerElement(xAxisCanvas, cv::Size{canvasSize.width, 0}, AlignmentType::WidthOnly);
    xAxisCanvas.copyTo(m_canvas(cv::Rect(0, canvasRowCounter, xAxisCanvas.cols, xAxisCanvas.rows)));

    //Generate the histogram graph element.
    return m_canvas;
}

cv::Size Histogram::calculateMinimumCanvasSize(const cv::Size& titleCanvasSize, const cv::Size& xAxisCanvasSize)
{
    //Determine the space required for axis number texts
    m_xAxisTextSize = allocateNumericTextSpace(DEFAULT_AXIS_NUMBER_SIZE, 1.25, m_precision_x);
    m_yAxisTextSize = allocateNumericTextSpace(DEFAULT_AXIS_NUMBER_SIZE, 1.25, m_precision_y);

    const cv::Size minimumHistogramSize{static_cast<int>(m_bins.size()), MINIMUM_HISTOGRAM_HEIGHT};
    const int histogramWidthWithyAxis = minimumHistogramSize.width + m_yAxisTextSize.width;

    //Combine minimum sizes
    const int totalHeight = (2 * CANVAS_HEIGHT_PADDING) + titleCanvasSize.height + PADDING_TITLE_HISTOGRAM + minimumHistogramSize.height +
                            m_xAxisTextSize.height + PADDING_HISTOGRAM_XAXIS + xAxisCanvasSize.height;
    const int totalWidth = std::max({titleCanvasSize.width, histogramWidthWithyAxis, xAxisCanvasSize.width}) + (2 * CANVAS_WIDTH_PADDING);

    return cv::Size{totalWidth, totalHeight};
}

cv::Mat Histogram::generateHistogramCanvas(const int titleCanvasHeight, const int xAxisCanvasHeight)
{
    //Create a histogram canvas with proper paddings
    const auto& [canvasWidth, canvasHeight] = m_canvas.size();
    const int histogramWidth = canvasWidth - (2 * CANVAS_WIDTH_PADDING) - yAxisTextWidth();
    const int histogramHeight = canvasHeight - totalHeightPadding() - titleCanvasHeight - xAxisTextHeight() - xAxisCanvasHeight;
    cv::Mat out(histogramHeight + xAxisTextHeight(), histogramWidth + yAxisTextWidth(), CV_8UC3, white);
    cv::Mat histogramCanvas = out(cv::Rect(m_yAxisTextSize.width + LENGTH_AXIS_LINE, 0, histogramWidth, histogramHeight));

    //Draw a rectangle around histogram to indicate the area
    cv::rectangle(histogramCanvas, cv::Rect(0, 0, histogramCanvas.cols, histogramCanvas.rows), black, 1, cv::LINE_AA);

    //Normalize histogram values to fit the histogram canvas
    constexpr double PADDING_MAX_HEIGHT_PERCENTAGE = 0.95;
    const size_t maxCount = *std::max_element(m_histogram.begin(), m_histogram.end());
    const int histogramHeight_padded = histogramHeight * PADDING_MAX_HEIGHT_PERCENTAGE;
    const auto lambda_normalizeBinHeight = [maxCount, histogramHeight_padded](const size_t curHistogram) -> int { return static_cast<int>(histogramHeight_padded * curHistogram / maxCount); };
    const std::vector<int> histogram_normalized = PlotUtils::vector_comprehension(m_histogram.cbegin(), m_histogram.cend(), lambda_normalizeBinHeight);

    //Determine the range of each bin
    const int binPixelWidth = histogramWidth / m_bins.size();

    //This counter keeps track of the current x-Axis position of the histogram.
    //Start point is the half of the remainder of the previous division to center the histogram
    const int binsStartPixel = (histogramWidth - (binPixelWidth * static_cast<int>(m_bins.size()))) / 2;
    int binPixelCounter = binsStartPixel;

    for(const size_t currentHistogram: histogram_normalized){
        //Define a rectangle that represents the location of the current bin and paint it black
        const cv::Rect binArea(binPixelCounter, histogramHeight - static_cast<int>(currentHistogram), binPixelWidth, static_cast<int>(currentHistogram));
        histogramCanvas(binArea).setTo(black);

        // Increment counter to place next bin
        binPixelCounter += binPixelWidth;
    }

    //Prepare the axis numbers
    const size_t yAxisStartPixel = histogramHeight - histogramHeight_padded;
    addAxis(out, binsStartPixel, yAxisStartPixel, {*m_bins.cbegin(), *(m_bins.cend() - 1)}, {0, maxCount});

    return out;
}

int Histogram::totalHeightPadding() const
{
    return (2 * CANVAS_HEIGHT_PADDING) + PADDING_TITLE_HISTOGRAM + PADDING_HISTOGRAM_XAXIS;
}


