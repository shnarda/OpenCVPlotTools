#include "plotelementbase.h"
#include "PlotUtils.h"
#include <iomanip>
#include <sstream>

using namespace PainterConstants;

//allocateTextSpace constant expressions
constexpr uint32_t TEXT_CANVAS_HEIGHT_NORM = 40;
constexpr uint32_t SPACE_CHAR_WIDTH_START = 30;
constexpr uint32_t LETTER_WIDTH_START = 20;

//addAxis constant expressions
constexpr size_t MINIMUM_PIXELS_BETWEEN_AXES = 15;
constexpr size_t NUMBER_OF_AXES = 6;
constexpr size_t LENGTH_AXIS_LINE = 5;
constexpr int OFFSET_TEXT_LINE = 10;

std::string PlotElementBase::getText(const TextField field) const
{
    switch (field) {
    case TextField::Title: return m_title;
    case TextField::XAxis: return m_xAxisText;
    case TextField::YAxis: return m_yAxisText;
    }
    throw std::runtime_error("Unknown text field type has been encountered");
}

void PlotElementBase::setText(const TextField component, const std::string& text, const float textSize, const cv::Scalar color)
{
    switch (component) {
    case TextField::Title:
        m_title = text;
        m_titleColor = color;
        m_titleSize = textSize * DEFAULT_TITLE_SIZE;
        break;
    case TextField::XAxis:
        m_xAxisText = text;
        m_xAxisColor = color;
        m_xAxisSize = textSize * DEFAULT_XAXIS_SIZE;
        break;
    case TextField::YAxis:
        m_yAxisText = text;
        m_yAxisColor = color;
        m_yAxisSize = textSize * DEFAULT_YAXIS_SIZE;
    }
}

void PlotElementBase::setText(const TextField component, std::string &&text, const float textSize, const cv::Scalar color)
{
    switch (component) {
    case TextField::Title:
        m_title = std::move(text);
        m_titleColor = color;
        m_titleSize = textSize * DEFAULT_TITLE_SIZE;
        break;
    case TextField::XAxis:
        m_xAxisText = std::move(text);
        m_xAxisColor = color;
        m_xAxisSize = textSize * DEFAULT_XAXIS_SIZE;
        break;
    case TextField::YAxis:
        m_yAxisText = std::move(text);
        m_yAxisColor = color;
        m_yAxisSize = textSize * DEFAULT_YAXIS_SIZE;
        break;
    default: break;
    }
}

void PlotElementBase::setPrecision(const AxisType axisType, const uint8_t precision)
{
    switch (axisType) {
    case AxisType::XAxis: m_precision_x = precision;
    case AxisType::YAxis: m_precision_y = precision;
    }
}

cv::Size PlotElementBase::allocateTextSpace(const float_t fontSize, const std::string_view text)
{
    const int paddingSize = 10 * fontSize;
    const auto countSpaces = std::count(text.begin(), text.end(), ' ');
    const uint32_t countLetters = text.length() - countSpaces;

    //These pixel sizes will give a rough estimation for the text canvas shape. The final canvas length will be decided from the resulting canvas
    const uint32_t canvasLength = (SPACE_CHAR_WIDTH_START * countSpaces) + (LETTER_WIDTH_START * countLetters) * fontSize + (2* paddingSize);
    const uint32_t canvasHeight = TEXT_CANVAS_HEIGHT_NORM * fontSize;

    return cv::Size(canvasLength, canvasHeight);
}

void PlotElementBase::centerElement(cv::Mat &target, const cv::Size &centerArea, const AlignmentType alignmentType)
{
    //Check for the illegal conditions
    if(target.empty())
        throw std::runtime_error("Target matrix cannot be empty");
    else if(target.type() != CV_8UC3)
        throw std::runtime_error("Centering operation is only meant to be used on 8UC3 type cv::Mat arrays (aka. plot related elements)");

    //Determine the new shape base on the selected alignment type
    const int newHeight = (alignmentType == AlignmentType::HeightOnly || alignmentType == AlignmentType::WholeShape)? centerArea.height : target.rows;
    const int newWidth = (alignmentType == AlignmentType::WidthOnly || alignmentType == AlignmentType::WholeShape)? centerArea.width : target.cols;

    //Calculate the paddings for centering the element
    const int paddingRows = (newHeight - target.rows) / 2;
    const int paddingCols = (newWidth - target.cols) / 2;

    if(newHeight < target.rows || newWidth < target.cols)
        throw std::runtime_error("New shape to be aligned has dimensions smaller than the original shape");

    //Reshape the target matrix
    cv::Mat centered(newHeight, newWidth, CV_8UC3, white);
    target.copyTo(centered({paddingCols, paddingRows, target.cols, target.rows}));
    target = centered;
}

cv::Mat PlotElementBase::centerElement(const cv::Mat &target, const cv::Size &centerArea, const AlignmentType alignmentType)
{
    //Check for the illegal conditions
    if(target.empty())
        throw std::runtime_error("Target matrix cannot be empty");
    else if(target.type() != CV_8UC3)
        throw std::runtime_error("Centering operation is only meant to be used on 8UC3 type cv::Mat arrays (aka. plot related elements)");

    //Determine the new shape base on the selected alignment type
    const int newHeight = (alignmentType == AlignmentType::HeightOnly || alignmentType == AlignmentType::WholeShape)? centerArea.height : target.rows;
    const int newWidth = (alignmentType == AlignmentType::WidthOnly || alignmentType == AlignmentType::WholeShape)? centerArea.width : target.cols;

    //Calculate the paddings for centering the element
    const int paddingRows = (newHeight - target.rows) / 2;
    const int paddingCols = (newWidth - target.cols) / 2;

    if(newHeight < target.rows || newWidth < target.cols)
        throw std::runtime_error("New shape to be aligned has dimensions smaller than the original shape");

    //Apply partial copying
    cv::Mat centered(newHeight, newWidth, CV_8UC3, white);
    target.copyTo(centered({paddingCols, paddingRows, target.cols, target.rows}));
    return centered;
}

void PlotElementBase::addAxis(cv::Mat &plotElement, const uint32_t startPixel_x, const uint32_t startPixel_y, const AxisRange range_x, const AxisRange range_y)
{
    //Constants that will repeteadly be used
    const int BOTTOM_XAXIS = plotElement.rows - 1;
    const int LINE_END_XAXIS = BOTTOM_XAXIS - LENGTH_AXIS_LINE;

    //Start with determining the numbers to be placed on the element
    const size_t numberofAxes_x = std::min(static_cast<size_t>((plotElement.cols - startPixel_x) / MINIMUM_PIXELS_BETWEEN_AXES), NUMBER_OF_AXES);
    const std::vector<double> xAxisNumbers(PlotUtils::linspace(range_x.first, range_x.second, numberofAxes_x));

    //Place each number for the x-axis
    int xAxisPosCounter = startPixel_x;
    for(const double currentNumber: xAxisNumbers){
        cv::line(plotElement, {xAxisPosCounter, BOTTOM_XAXIS}, {xAxisPosCounter, LINE_END_XAXIS}, cv::LINE_AA);

        //Prepare the text to be placed
        std::stringstream stream;
        stream << std::fixed << std::setprecision(m_precision_x) << std::scientific << currentNumber;
        std::string stringNumber = stream.str();
        cv::putText(plotElement, stringNumber,{xAxisPosCounter - OFFSET_TEXT_LINE, LINE_END_XAXIS - 2}, 1, 0.5, PainterConstants::blue, 1, cv::LINE_AA);
        xAxisPosCounter += (plotElement.cols - startPixel_x) / numberofAxes_x;
    }

    //Apply similar precedure for y-axis. y-axis numbers should be reverse ordered
    const size_t numberofAxes_y = std::min(static_cast<size_t>((plotElement.rows - startPixel_y) / MINIMUM_PIXELS_BETWEEN_AXES), NUMBER_OF_AXES);
    const std::vector<double> yAxisNumbers(PlotUtils::linspace(range_y.second, range_y.first, numberofAxes_y));


    //Place each number for the y-axis.
    int yAxisPosCounter = startPixel_y;
    for(const double currentNumber: yAxisNumbers){
        cv::line(plotElement, {0, yAxisPosCounter}, {LENGTH_AXIS_LINE, yAxisPosCounter}, cv::LINE_AA);

        //Prepare the text to be placed
        std::stringstream stream;
        stream << std::fixed << std::setprecision(m_precision_y) << std::scientific << currentNumber;
        std::string stringNumber = stream.str();
        cv::putText(plotElement, stringNumber,{LENGTH_AXIS_LINE + 2, yAxisPosCounter}, 1, 0.5, PainterConstants::blue, 1, cv::LINE_AA);
        yAxisPosCounter += (plotElement.rows - startPixel_y) / (numberofAxes_y - 1);
    }
}

cv::Mat PlotElementBase::generateText(const float_t fontSize, const std::string_view text, const cv::Scalar textColor)
{
    //White text color messes up the algorithm
    if(textColor == white)
        throw(std::runtime_error("White cannot be chosen as the text color"));

    //Estimate the space needed for the text with confident vertical margin. This margin will be trimmed soon
    const cv::Size allocatedSpace = allocateTextSpace(fontSize, text);
    cv::Mat canvas = cv::Mat(allocatedSpace, CV_8UC3, white);

    const int marginSize = 10 * fontSize;
    cv::putText(canvas, cv::String{text.data(), text.size()}, cv::Point{marginSize, static_cast<int>(allocatedSpace.height - marginSize)}, font, fontSize, textColor, 1, cv::LINE_AA);

    uint32_t firstInstance = 0;
    for(uint32_t curCol=canvas.cols - 1; curCol>0; curCol--){
        cv::Mat curColumn = canvas.col(curCol);
        if(!cv::checkRange(curColumn, true, nullptr, 255)){
            firstInstance = curCol;
            break;
        }
    }

    return canvas.colRange(0,  std::min(firstInstance + marginSize, static_cast<uint32_t>(canvas.cols)));
}
