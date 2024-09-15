#ifndef SUBPLOT_H
#define SUBPLOT_H
#include "plotelementbase.h"
#include "histogram.h"
#include "colormap.h"
#include "emptyspace.h"


class Subplot : public PlotElementBase
{
public:
    Subplot(const std::vector<Plottable>& plotElements, const size_t rows, const size_t cols);

    cv::Mat generate();

    const Plottable& operator[](size_t index) const {return m_plotElements[index];};

    //Precision won't be involved for this class
    void setPrecision(const AxisType axisType, const uint8_t precision) = delete;

    Subplot clone() const;
private:
    size_t m_rows;
    size_t m_cols;
    std::vector<Plottable> m_plotElements;

private:
    cv::Size calculateMinimumCanvasSize(const cv::Size& titleCanvasSize, const int totalRowHeight, const int totalColWidth) const;
    std::vector<int> getLargestRows() const;
    std::vector<int> getLargestColumns() const;
};

#endif // SUBPLOT_H
