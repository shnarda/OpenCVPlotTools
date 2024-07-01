#ifndef PLOTELEMENTBASE_H
#define PLOTELEMENTBASE_H

#include "opencv2/core/types.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>
#include <opencv2/core/mat.hpp>


using AxisRange = std::pair<double, double>;

enum class TextField{Title, XAxis, YAxis, Legend};

namespace PlotUtils{
    template <typename T>
    struct is_iterator {
        template <typename U>
        static uint8_t test(typename std::iterator_traits<U>::pointer* x);

        template <typename U>
        static uint16_t test(U* x);

        static const bool value = sizeof(test<T>(nullptr)) == 1;
    };

    /**
    * @brief This utility function applies the function given by the "functor" on given input ranges and returns a vector that has the elements from each returned value
    * @param begin: Any iterator = begin range of the iterator
    * @param end: Any iterator = end range of the iterator
    * @param functor: functor that takes the value type of the input iterators.The function should return a value
    * @return A vector that has the matching elements from each iterating element that has been applied to "functor"
    */
    template<typename It, typename Lambda>
    static auto vector_comprehension(const It begin,const It end,const Lambda functor)
    {
        //Template parameter "It" should be an iterator and the template parameter "Lambda" should take the iterating value type as a parameter
        static_assert(is_iterator<It>::value);
        static_assert(std::is_invocable<Lambda, typename std::iterator_traits<It>::value_type>::value);

        std::vector<typename std::iterator_traits<It>::value_type> out;
        out.reserve(std::distance(begin, end));
        std::transform(begin, end, std::back_inserter(out), functor);
        return out;
    };


    /**
    * @brief This utility function generates a vector that has the linearly distributed elements from "start" to "end"
    * @param start: start point of the vector. The value is always included
    * @param end: end point of the vector. The value is always included
    * @param t_count: number of elements of the output vector. If it's not given, the count will be the integer distance between the start and end
    * @return A vector that has the linearly distributed elements from "start" to "end"
    */
    static std::vector<double> linspace(const float start, const float end, const std::optional<size_t> t_count = {})
    {
        const size_t count = (t_count)? *t_count : (std::abs(end - start) + 1);

        //Handle illegal cases
        if(count == 0)
            throw(std::invalid_argument("Range cannot be zero"));

        const float inc = (end - start) / std::max(count - 1, static_cast<size_t>(1));
        float curValue = start - inc;
        const auto incFloat = [&curValue, inc]() {return curValue += inc;};

        std::vector<double> out(count, 0);
        std::generate(out.begin(), out.end(), incFloat);
        return out;
    };
}



template <typename T>
class PlotElementBase
{
public:
    //Getters
    cv::Size getCanvasSize() const {return canvasSize;};
    std::string title() const {return m_title;};
    cv::Mat& canvas() {return m_canvas;};
    std::string xAxisText() const{return m_xAxisText;};
    std::string yAxisText() const{return m_yAxisText;};

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

protected:
    static constexpr uint32_t CANVAS_WIDTH_PADDING = 10;
    static constexpr uint32_t CANVAS_HEIGHT_PADDING = 10;

private:
    PlotElementBase() = default;
    friend T;

    cv::Size_<uint16_t> canvasSize{640, 512};
    cv::Mat m_canvas;
    std::string m_title;
    std::string m_xAxisText;
    std::string m_yAxisText;
};

#endif // PLOTELEMENTBASE_H
