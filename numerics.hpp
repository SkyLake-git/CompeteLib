#pragma once
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

// nPr
template<Arithmetic T>
T permutation(T n, T r) {
    T res = n;

    for (int i = 1; i < r; ++i) {
        res *= n - i;
    }

    return res;
}

// nCr
template<Arithmetic T>
T combination(T n, T r) {
    if (r < 0 || r > n) {
        return 0;
    }

    if (r > n - r) {
        r = n - r;
    }

    T res = 1;

    for (T i = 1; i <= r; ++i) {
        res = res * (n - i + 1) / i;
    }

    return res;
}

// n+1C2
template<Arithmetic T>
T partitions(T n) {
    return n * (n + 1) / 2;
}

template<Arithmetic T>
T sum_seq(T a, T l, T n) {
    return (a + l) * n / 2;
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

// bit の nbit 目が 1 かどうか判定する
// 0 <= nbit < 63
inline bool bit_has(long long bit, int nbit) {
    assert(nbit >= 0 && nbit < 63);
    return bit & (1LL << nbit);
}

// bit の nbit 目を立てる
// 0 <= nbit < 63
inline long long bit_set(long long bit, int nbit) {
    assert(nbit >= 0 && nbit < 63);
    return bit | (1LL << nbit);
}

// bit の nbit 目を 0 にする
// 0 <= nbit < 63
inline long long bit_reset(long long bit, int nbit) {
    assert(nbit >= 0 && nbit < 63);
    return bit & ~(1LL << nbit);
}
