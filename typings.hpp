#ifndef ATCODERC_TYPINGS_HPP
#define ATCODERC_TYPINGS_HPP
#include <concepts>
#include <type_traits>

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template<typename F, typename... Args>
concept Callable = std::invocable<F, Args...>;

enum DistanceAlgo {
    Euclidean,
    Manhattan
};

#endif //ATCODERC_TYPINGS_HPP
