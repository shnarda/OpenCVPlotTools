#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#include <algorithm>
#include <iterator>
#include <optional>
#include <type_traits>
#include <vector>
#endif // PLOTUTILS_H


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
    std::generate(out.begin(), out.end() - 1, incFloat);
    *(out.end() - 1) = end;
    return out;
};

template<size_t S>
static constexpr std::array<double, S> linspace(const float start, const float end)
{
    //Check for the illegal condition
    static_assert(S != 0, "Range cannot be zero");

    constexpr float inc = (end - start) / std::max(S - 1, static_cast<size_t>(1));
    float curValue = start - inc;
    constexpr auto incFloat = [&curValue, inc]() {return curValue += inc;};

    std::array<double, S> out{};
    out[0] = 0;
    std::generate(out.begin(), out.end() - 1, incFloat);
    *(out.end() - 1) = end;
    return out;
};


}
