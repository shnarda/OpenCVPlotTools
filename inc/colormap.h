#ifndef COLORMAP_H
#define COLORMAP_H
#include "plotelementbase.h"
#include <optional>

class Colormap : public PlotElementBase
{
public:
    /**
    * @brief Default constructor variant with the fixed colormap range of 0-255
    * @param target: The target matrix that the colormap will be applied to. The target matrix should be either CV_8U, CV_8UC3 or CV_8UC1
    * @param colormapType: The colormap to apply, see ColormapTypes from the OpenCV library
    */
    Colormap(const cv::Mat& target, const cv::ColormapTypes colormapType);

    /**
    * @brief Constructor variant with the custom matrix ranges. Normalization to 0-255 will be done internally
    * @param target: The target matrix that the colormap will be applied to. The target matrix can be any type
    * @param colormap_min: Minimum border of the colormap. If it's nullopted, the value will take the smallest value within the matrix
    * @param colormap_max: Maximum border of the colormap. If it's nullopted, the value will take the largest value within the matrix
    * @param colormapType: The colormap to apply, see ColormapTypes from the OpenCV library
    */
    Colormap(const cv::Mat& target,
             const std::optional<double> colormap_min={},
             const std::optional<double> colormap_max={},
             const cv::ColormapTypes colormapType=cv::ColormapTypes::COLORMAP_JET);

    /**
    * @brief Generates the colormap canvas by using the parameters that have been given.
    * @return The colormap canvas that has been generated.
    */
    cv::Mat generate();

    void setColorbarPrecision(const uint8_t precision) {m_colorbarPrecision = precision;};

    Colormap clone() const;

private:
    cv::Size calculateMinimumCanvasSize(const cv::Size titleCanvasSize, const cv::Size xAxisCanvasSize);

    cv::Mat generateColormapCanvas(const int titleCanvasHeight, const int xAxisCanvasHeight) const;
    cv::Mat generateColorbar(const int colormapHeight) const;

    cv::Mat resizeColormap(const int titleCanvasHeight, const int xAxisCanvasHeight) const;

    int colorbarTotalWidth() const;
    int totalHeightPadding() const;

private:
    cv::Mat m_colormap;

    cv::ColormapTypes m_colormapType;

    std::pair<double, double> m_colormapRange{};

    uint8_t m_colorbarPrecision = 1;

    cv::Size m_colorbarTextSize{};
};

#endif // COLORMAP_H
