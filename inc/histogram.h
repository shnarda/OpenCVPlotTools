#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "paintertools.h"
#include "plotelementbase.h"
#include <optional>


class Histogram : public PlotElementBase<Histogram>
{
public:
    /**
    * @brief Constructor variant, where the histogram data is given directly by the parameters
    * @param histogram: histogram distribution vector
    * @param bins: bins vector that represents the quantized values that histogram has been counted
    */
    explicit Histogram(const std::vector<size_t>& histogram, const std::vector<double>& bins);
    explicit Histogram(std::vector<size_t>&& histogram, std::vector<double>&& bins);

    /**
    * @brief Constructor variant, where the histogram data is given directly by the parameters. Bins are determined automatically with 1:histogram.size()
    * @param histogram: histogram distribution vector
    */
    explicit Histogram(const std::vector<size_t>& histogram);
    explicit Histogram(std::vector<size_t>&& histogram);

    /**
    * @brief Constructor variant, where the histogram data is given directly by the parameters. Bins are determined by the given range, linerly spaced
    * @param histogram: histogram distribution vector
    * @param binsStart: first value of the bin range
    * @param binEnd: last value of the bin range. If it's not given, it will take the following value -> binStart + histogram.size()
    */
    explicit Histogram(const std::vector<size_t>& histogram, const double binStart, const std::optional<double> binEnd);
    explicit Histogram(std::vector<size_t>&& histogram, const double binStart, const std::optional<double> binEnd);

    /**
    * @brief Constructor variant, where the histogram should be calculated inside the class.
    * @param inArray: OpenCV array that will be calculated
    * @param binsSize: Number of the bins that represent the quantized values that histogram has been counted. If it's not given, binSize will be the difference between the minimum
    * and maximum value within the array
    * @param binStart: first value of the bin range. If it's not given, it will take the minimum value within the array
    * @param binEnd: last value of the bin range. If it's not given, it will take the maximum value within the array
    */
    explicit Histogram(const cv::Mat &inArray, const std::optional<int> binSize = {}, const std::optional<double> binStart = {}, const std::optional<double> binEnd = {});

    //Getters
    const std::vector<size_t>& getHistogram() const {return m_histogram;};
    const std::vector<double>& getBins() const {return m_bins;};

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
    * @brief Generates the histogram canvas by using the parameters that have been given.
    * @return The histogram canvas that has been generated.
    */
    cv::Mat generate();

private:
    std::vector<size_t> m_histogram;
    std::vector<double> m_bins;

    float m_titleSize;
    float m_xAxisSize;

    cv::Scalar m_titleColor = PainterConstants::black;
    cv::Scalar m_xAxisColor = PainterConstants::black;

    cv::Size_<uint16_t> calculateMinimumCanvasSize();

    cv::Mat generateHistogramCanvas(const int titleCanvasHeight, const int xAxisCanvasHeight);
};

#endif // HISTOGRAM_H
