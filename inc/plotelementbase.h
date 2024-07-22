#ifndef PLOTELEMENTBASE_H
#define PLOTELEMENTBASE_H

#include "opencv2/core/types.hpp"
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc.hpp"



namespace PainterConstants{
inline const cv::Scalar white{255, 255, 255};
inline const cv::Scalar black{0, 0, 0};
inline const cv::Scalar blue{255, 0, 0};
inline const cv::Scalar green{0, 255, 0};
inline const cv::Scalar red{0, 0, 255};
inline constexpr auto font = cv::HersheyFonts::FONT_HERSHEY_COMPLEX;
}

using AxisRange = std::pair<double, double>;

enum class TextField{Title, XAxis, YAxis};
enum class AxisType{XAxis, YAxis};

class PlotElementBase
{
public:
    //Getters
    cv::Size getCanvasSize() const {return canvasSize;};
    std::string getText(const TextField field) const;
    cv::Mat& canvas() {return m_canvas;};

    /**
    * @brief Sets the size of the generated canvas. If it won't be set, the class attempts to generate
    * the minimum allowed canvas size. This also happens if the user sets the the canvas size too small for
    * the class to generate.
    * @param width: Width of the target canvas
    * @param height: Height of the target canvas
    */
    void setCanvasSize(const uint16_t width, const uint16_t height){ canvasSize.height = height; canvasSize.width = width;}

    /**
    * @brief cv::Size variant of the setcanvasSize function
    * @param size: shape of the canvas
    */
    void setCanvasSize(const cv::Size_<uint16_t> size) {canvasSize = size;};

    /**
    * @brief Sets the text field that has been provided from the parameter "component"
    * @param component: Specifies the target location of the text to be set.
    * @param text: the text to be set
    * @param textSize: Determines the size of the text which always have default value of 1. Setting it 2 doubles the default text size.
    * @param color: The color of the text, which is represented by the BGR value.
    */
    void setText(const TextField component, const std::string& text, const float textSize=1, const cv::Scalar color=PainterConstants::black);
    void setText(const TextField component, std::string&& text, const float textSize=1, const cv::Scalar color=PainterConstants::black);

    /**
    * @brief Sets the text field that has been provided from the parameter "component"
    * @param axisType: Specifies the target axis type to be set.
    * @param precision: Floating point precision to be set
    */
    void setPrecision(const AxisType axisType, const uint8_t precision);

protected:
    //The base class should never be constructed induvidually
    PlotElementBase() = default;

    [[nodiscard]] static cv::Mat generateText(const float_t fontSize, const std::string_view text, const cv::Scalar textColor=PainterConstants::black);

    [[nodiscard]] static cv::Mat generateNumericText(const float_t fontSize, const double_t number, const uint8_t precision);

    [[nodiscard]] static cv::Size allocateNumericTextSpace(const float_t fontSize, const double_t number, const uint8_t precision);

    enum class AlignmentType{WidthOnly, HeightOnly, WholeShape};
    static void centerElement(cv::Mat& centerTarget, const cv::Size_<size_t>& centerArea, const AlignmentType alignmentType);
    [[nodiscard]] static cv::Mat centerElement(const cv::Mat& centerTarget, const cv::Size_<size_t>& centerArea, const AlignmentType alignmentType);

    void addAxis(cv::Mat& plotElement, const uint32_t startPixel_x, const uint32_t startPixel_y, const AxisRange range_x, const AxisRange range_y) const;

    //protected getters
    int yAxisTextWidth() const {return m_yAxisTextSize.width + LENGTH_AXIS_LINE;};
    int xAxisTextHeight() const {return m_xAxisTextSize.height + LENGTH_AXIS_LINE;};

protected:
    //Compile time constants
    static constexpr int CANVAS_WIDTH_PADDING = 10;
    static constexpr int CANVAS_HEIGHT_PADDING = 10;
    static constexpr float_t DEFAULT_TITLE_SIZE = 0.8;
    static constexpr float_t DEFAULT_XAXIS_SIZE = 0.4;
    static constexpr float_t DEFAULT_YAXIS_SIZE = 0.4;
    static constexpr float_t DEFAULT_AXIS_NUMBER_SIZE = 0.3;
    static constexpr int LENGTH_AXIS_LINE = 5;

    //Common plot element members
    cv::Size canvasSize{640, 512};
    cv::Mat m_canvas{};
    std::string m_title{};
    std::string m_xAxisText{};
    std::string m_yAxisText{};
    cv::Scalar m_titleColor = PainterConstants::black;
    cv::Scalar m_xAxisColor = PainterConstants::black;
    cv::Scalar m_yAxisColor = PainterConstants::black;
    uint8_t m_precision_x = 1;
    uint8_t m_precision_y = 1;
    float m_titleSize = DEFAULT_TITLE_SIZE;
    float m_xAxisSize = DEFAULT_XAXIS_SIZE;
    float m_yAxisSize = DEFAULT_YAXIS_SIZE;
    cv::Size m_xAxisTextSize{};
    cv::Size m_yAxisTextSize{};


};

#endif // PLOTELEMENTBASE_H
