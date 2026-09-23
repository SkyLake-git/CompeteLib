#ifndef ATCODERC_VECTOR_HPP
#define ATCODERC_VECTOR_HPP
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "lib/typings.hpp"
#include "lib/utils.hpp"

template<Arithmetic T>
struct vec2 {
    T x{};
    T y{};

    vec2() = default;

    vec2(T x, T y) {
        this->x = x;
        this->y = y;
    }

    explicit vec2(std::pair<T, T> &a) {
        this->x = a.first;
        this->y = a.second;
    }

    long double distance(vec2 &another) {
        return std::hypotl(static_cast<long double>(this->x - another.x),
                           static_cast<long double>(this->y - another.y));
    }

    long double length() {
        return std::hypotl(static_cast<long double>(this->x), static_cast<long double>(this->y));
    }

    vec2 operator+(const vec2 &another) const {
        return vec2{this->x + another.x, this->y + another.y};
    }

    vec2 operator-(const vec2 &another) const {
        return vec2{this->x - another.x, this->y - another.y};
    }

    vec2 operator*(const vec2 &another) const {
        return vec2{this->x * another.x, this->y * another.y};
    }

    vec2 operator/(const vec2 &another) const {
        return vec2{this->x / another.x, this->y / another.y};
    }

    vec2 &operator+=(const vec2 &another) {
        this->x += another.x;
        this->y += another.y;
        return *this;
    }

    vec2 &operator-=(const vec2 &another) {
        this->x -= another.x;
        this->y -= another.y;
        return *this;
    }

    vec2 &operator*=(const vec2 &another) {
        this->x *= another.x;
        this->y *= another.y;
        return *this;
    }

    vec2 &operator/=(const vec2 &another) {
        this->x /= another.x;
        this->y /= another.y;
        return *this;
    }

    bool operator==(const vec2 &another) const {
        return this->x == another.x && this->y == another.y;
    }

    bool operator!=(const vec2 &another) const {
        return !(*this == another);
    }
};

struct vec2i : vec2<int> {
    using vec2::vec2;
};

struct vec2l : vec2<long long> {
    using vec2::vec2;
};

template<DistanceAlgo C, Arithmetic T, Arithmetic R>
R calc_vec_distance(const vec2<T> &a, const vec2<T> &b) {
    const R dx = static_cast<R>(a.x) - static_cast<R>(b.x);
    const R dy = static_cast<R>(a.y) - static_cast<R>(b.y);
    if constexpr (C == Euclidean) {
        return dx * dx + dy * dy;
    } else if constexpr (C == Manhattan) {
        return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
    } else {
        throw std::logic_error("");
    }
}

template<DistanceAlgo C, Arithmetic T, Arithmetic R>
bool eval_vec_distance(const vec2<T> &a, const vec2<T> &b, R range) {
    if constexpr (C == Euclidean) {
        return calc_vec_distance<C, T, R>(a, b) <= range * range;
    } else {
        return calc_vec_distance<C, T, R>(a, b) <= range;
    }
}

/**
 *
 * @tparam X [0, x)
 * @tparam Y [0, y)
 */
template<int X, int Y>
struct vec2i_hasher {
    // ReSharper disable once CppMemberFunctionMayBeStatic
    long long encode_from(int x, int y) {
        assert(0 <= x && x < X);
        assert(0 <= y && y < Y);
        return static_cast<long long>(x) * Y + y;
    }

    long long encode(const vec2<int> &v) {
        return encode_from(v.x, v.y);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    vec2<int> decode(long long value) {
        assert(0 <= value && value < static_cast<long long>(X) * Y);

        return {
            static_cast<int>(value / Y),
            static_cast<int>(value % Y),
        };
    }
};

#endif //ATCODERC_VECTOR_HPP
