#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "plotelementbase.h"
#include <optional>


class Histogram : public PlotElementBase
{
public:
    /**
    * @brief Constructor variant, where the histogram data is given directly by the parameters
    * @param histogram: histogram distribution vector
    * @param bins: bins vector that represents the quantized values that histogram has been counted
    */
    explicit Histogram(const std::vector<size_t>& histogram, const std::vector<float>& bins);
    explicit Histogram(std::vector<size_t>&& histogram, std::vector<float>&& bins);

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
    explicit Histogram(const std::vector<size_t>& histogram, const float binStart, const std::optional<float> binEnd);
    explicit Histogram(std::vector<size_t>&& histogram, const float binStart, const std::optional<float> binEnd);

    /**
    * @brief Constructor variant, where the histogram should be calculated inside the class.
    * @param inArray: OpenCV array that will be calculated
    * @param binsSize: Number of the bins that represent the quantized values that histogram has been counted. If it's not given, binSize will be the difference between the minimum
    * and maximum value within the array
    * @param binStart: first value of the bin range. If it's not given, it will take the minimum value within the array
    * @param binEnd: last value of the bin range. If it's not given, it will take the maximum value within the array
    */
    explicit Histogram(const cv::Mat &inArray, const std::optional<int> binSize = {}, const std::optional<float> binStart = {}, const std::optional<float> binEnd = {});

    //Getters
    const std::vector<size_t>& getHistogram() const {return m_histogram;};
    const std::vector<float>& getBins() const {return m_bins;};

    Histogram clone() const;

    /**
    * @brief Generates the histogram canvas by using the parameters that have been given.
    * @return The histogram canvas that has been generated.
    */
    cv::Mat generate();

private:
    cv::Size calculateMinimumCanvasSize(const cv::Size& titleCanvasSize, const cv::Size& xAxisCanvasSize);

    cv::Mat generateHistogramCanvas(const int titleCanvasHeight, const int xAxisCanvasHeight);

    int totalHeightPadding() const;

private:
    std::vector<size_t> m_histogram;
    std::vector<float> m_bins;

};

#endif // HISTOGRAM_H
