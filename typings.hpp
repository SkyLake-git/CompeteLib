#ifndef ATCODERC_TYPINGS_HPP
#define ATCODERC_TYPINGS_HPP
#include <type_traits>

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

enum DistanceAlgo {
    Euclidean,
    Manhattan
};

#endif //ATCODERC_TYPINGS_HPP
