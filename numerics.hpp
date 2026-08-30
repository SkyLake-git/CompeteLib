#ifndef ATCODERC_NUMERICS_HPP
#define ATCODERC_NUMERICS_HPP
#include <cmath>
#include "lib/typings"

template<Arithmetic T>
T factorial(T a) {
    T res = 1;
    for (T i = a; i > 0; --i) {
        res *= i;
    }

    return res;
}

template<Arithmetic T>
T factorial_mod(T a, T mod) {
    T res = 1;
    for (T i = a; i > 0; --i) {
        res *= i;
        res %= mod;
    }

    return res;
}

template<Arithmetic T>
T probability(T a, T c) {
    T res = a;

    for (int i = 0; i < c; ++i) {
        res *= a - i;
    }

    return res;
}

// 繰り返し2乗法
template<Arithmetic R, Arithmetic T>
R pow_int(T a, T exponent) {
    R x = 1;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            x *= a;
        }

        a *= a;
        exponent >>= 1;
    }

    return x;
}

template<Arithmetic T>
T ceil_div(T a, T b) {
    if (b < 0)
        a *= -1, b *= -1;
    if (a <= 0)
        return a / b;

    return (a - 1) / b + 1;
}

template<Arithmetic T>
bool is_inside_bounds(T x, T y, T width, T height) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

inline long long ceil_ll(const long double a) {
    return static_cast<long long>(std::ceill(a));
}

inline long long floor_ll(const long double a) {
    return static_cast<long long>(std::floorl(a));
}


#endif //ATCODERC_NUMERICS_HPP
