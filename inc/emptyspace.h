#ifndef EMPTYSPACE_H
#define EMPTYSPACE_H
#include "plotelementbase.h"
#include <optional>

class EmptySpace : public PlotElementBase
{
public:
    EmptySpace() { canvasSize = cv::Size{ 0, 0 }; };

    cv::Mat generate() { m_canvas = cv::Mat{ canvasSize, CV_8UC3, PainterConstants::white }; return m_canvas; };

    EmptySpace clone() const { return *this; };
};

#endif // COLORMAP_H
