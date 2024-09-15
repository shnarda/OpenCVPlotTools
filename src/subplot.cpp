#include "subplot.h"
#include <numeric>




namespace {
    //We will clearly use constants from this namespace
    using namespace PainterConstants;

    constexpr int PADDING_TITLE_SUBPLOT = 10;


    auto getPlottableCanvasSize(Plottable element) -> cv::Size
    {
        const auto lambda_getCanvasSize = [](const auto& element)-> cv::Size {return element.getCanvasSize();};
        return std::visit(lambda_getCanvasSize, element);
    }

    auto getReshapedElement(const Plottable& element, const cv::Size newShape) -> cv::Mat
    {
        const auto lambda_getClone = [](const auto& element) -> Plottable {return element.clone(); };
        const auto lambda_reshapeElement = [newShape](auto& element) {element.setCanvasSize(newShape); element.generate(); };
        const auto lambda_getCanvas = [](const auto& element) -> cv::Mat {return element.canvas(); };

        //Clone the current element
        Plottable c_element = std::visit(lambda_getClone, element);

        //Reshape the current element
        std::visit(lambda_reshapeElement, c_element);

        return std::visit(lambda_getCanvas, c_element);
    }
}


Subplot::Subplot(const std::vector<Plottable> &plotElements, const size_t rows, const size_t cols) :
    m_rows(rows),
    m_cols(cols),
    m_plotElements(plotElements)
{
    //Total number of elements should always be equal to subplot rows * cols
    if(rows * cols != plotElements.size()){
        throw std::runtime_error("number of rows and columns should match with the number of plot elements");
    }
    if(plotElements.empty()){
        throw std::runtime_error("number of plot elements cannot be zero");
    }
}

auto Subplot::generate() -> cv::Mat
{
    //Generate all plot if they aren't previously been generated.
    const auto lambda_generate_if_empty = [](auto& curCanvas){ if(curCanvas.empty()){curCanvas.generate();} };
    for(Plottable& curCanvas : m_plotElements){
        std::visit(lambda_generate_if_empty, curCanvas);
    }

    //Determine the largest canvas size that can fit all available input plots
    const std::vector<int> largestRows = getLargestRows();
    const std::vector<int> largestColumns = getLargestColumns();

    auto a = largestColumns.cbegin();

    //Initialize a row counter
    int rowCounter = CANVAS_HEIGHT_PADDING;

    //Generate the title but don't place it on the canvas yet. Size of these canvases will determine the size of the main canvas
    cv::Mat titleCanvas = (m_title.empty()) ? cv::Mat() : generateText(m_titleSize, m_title, m_titleColor);

    //There is a lower limit on the sizes that a canvas can have
    const int totalColWidth = std::reduce(largestColumns.begin(), largestColumns.end());
    const int totalRowHeight = std::reduce(largestRows.begin(), largestRows.end());
    const cv::Size minimumCanvasSize = calculateMinimumCanvasSize(titleCanvas.size(), totalRowHeight, totalColWidth);
    canvasSize.height = std::max(canvasSize.height, minimumCanvasSize.height);
    canvasSize.width = std::max(canvasSize.width, minimumCanvasSize.width);

    //Generate a blank canvas
    m_canvas = cv::Mat(canvasSize, CV_8UC3, white);

    //This counter keeps track of the last row position on the canvas
    int canvasRowCounter = 0;

    //Add top padding to the row counter
    canvasRowCounter += CANVAS_HEIGHT_PADDING;

    //Reshape the previously generated title and place it on the canvas
    if (!titleCanvas.empty()) {
        centerElement(titleCanvas, cv::Size{ canvasSize.width, 0 }, AlignmentType::WidthOnly);
        titleCanvas.copyTo(m_canvas.rowRange(canvasRowCounter, canvasRowCounter + titleCanvas.rows));

        canvasRowCounter += titleCanvas.rows + PADDING_TITLE_SUBPLOT;
    }

    //Reshape each element, then place it on the canvas
    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            const Plottable& curElement = m_plotElements.at((r * m_cols) + c);
            cv::Mat& curCanvas = getReshapedElement(curElement, { largestColumns.at(c), largestRows.at(r)});

            const int accumulatedWidth = std::reduce(largestColumns.begin(), largestColumns.begin() + c);
            const int accumulatedHeight = std::reduce(largestRows.begin(), largestRows.begin() + r);
            const cv::Rect targetArea{CANVAS_WIDTH_PADDING + accumulatedWidth, canvasRowCounter + accumulatedHeight, largestColumns.at(c), largestRows.at(r)};
            curCanvas.copyTo(m_canvas(targetArea));
        }
    }

    return m_canvas;
}

cv::Size Subplot::calculateMinimumCanvasSize(const cv::Size &titleCanvasSize, const int totalRowHeight, const int totalColWidth) const
{
    //Combine minimum sizes
    const int titleHeight = (titleCanvasSize.empty())? 0 : titleCanvasSize.height + PADDING_TITLE_SUBPLOT;
    const int titleWidth = (titleCanvasSize.empty())? 0 : titleCanvasSize.width;
    const int totalHeight = totalRowHeight + titleHeight + (2 * CANVAS_HEIGHT_PADDING);
    const int totalWidth = std::max(titleWidth, totalColWidth) + (2 * CANVAS_WIDTH_PADDING);

    return cv::Size{totalWidth, totalHeight};
}

std::vector<int> Subplot::getLargestRows() const
{
    const auto lambda_compareVectorHeight = [](const Plottable first, const Plottable second) -> bool {
        return getPlottableCanvasSize(first).height < getPlottableCanvasSize(second).height;
    };
    std::vector<int>out;
    out.reserve(m_rows);

    for (auto it = m_plotElements.cbegin(); it != m_plotElements.end(); it += m_cols) {
        const int largestHeight = getPlottableCanvasSize(*std::max_element(it, it + m_cols, lambda_compareVectorHeight)).height;
        out.emplace_back(largestHeight);
    }
    return out;
}

std::vector<int> Subplot::getLargestColumns() const
{
    const auto lambda_compareVectorWidth = [](const Plottable* first, const Plottable* second) -> bool {
        return getPlottableCanvasSize(*first).width < getPlottableCanvasSize(*second).width;
    };

    std::vector<int>out;
    out.reserve(m_cols);

    for (int c = 0; c < m_cols; c++) {
        std::vector<const Plottable*> columnElements;
        columnElements.reserve(m_rows);

        for (int r = 0; r < m_rows; r++) {
            const int index = r * m_cols + c; 
            columnElements.push_back(&m_plotElements.at(index));
        }
        
        const int largestWidth = getPlottableCanvasSize(**std::max_element(columnElements.begin(), columnElements.end(), lambda_compareVectorWidth)).width;
        out.emplace_back(largestWidth);
    }
    return out;
}

Subplot Subplot::clone() const
{
    //Clone all cv::Mat types and copy everything else
    Subplot out(*this);
    out.m_canvas = m_canvas.clone();

    return out;
}


