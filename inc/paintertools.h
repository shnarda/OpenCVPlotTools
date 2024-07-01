#ifndef PAINTERTOOLS_H
#define PAINTERTOOLS_H

#include "opencv2/imgproc.hpp"
#include "plotelementbase.h"

namespace PainterConstants{
inline const cv::Scalar white{255, 255, 255};
inline const cv::Scalar black{0, 0, 0};
inline const cv::Scalar blue{255, 0, 0};
inline const cv::Scalar green{0, 255, 0};
inline const cv::Scalar red{0, 0, 255};
inline constexpr auto font = cv::HersheyFonts::FONT_HERSHEY_COMPLEX;
}

class Histogram;

//This is a utility class that should only be accessed by the friend classes
class PainterTools
{
public:
    //This is a utility class so delete all special member functions
    PainterTools() = delete;
    PainterTools(const PainterTools& other) = delete;
    PainterTools& operator=(const PainterTools& other) = delete;
    ~PainterTools() = delete;

private:
    enum class AlignmentType{WidthOnly, HeightOnly, WholeShape};

    friend Histogram;

    [[nodiscard]] static cv::Mat generateText(const float_t fontSize, const std::string_view text, const cv::Scalar textColor=PainterConstants::black);

    [[nodiscard]] static cv::Size allocateTextSpace(const float_t fontSize, const std::string_view text);

    static void centerElement(cv::Mat& centerTarget, const cv::Size& centerArea, const AlignmentType alignmentType);
    [[nodiscard]] static cv::Mat centerElement(const cv::Mat& centerTarget, const cv::Size& centerArea, const AlignmentType alignmentType);

    static void addAxis(cv::Mat& plotElement, const uint32_t startPixel_x, const uint32_t startPixel_y, const AxisRange range_x, const AxisRange range_y, const uint8_t precision_x, const uint8_t precision_y);
};

#endif // PAINTERTOOLS_H
