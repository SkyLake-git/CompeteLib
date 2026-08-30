#ifndef ATCODERC_VECTOR_HPP
#define ATCODERC_VECTOR_HPP
#include <complex>
#include <utility>

#include "typings.hpp"

template<Arithmetic T>
struct vec2 {
    T x{};
    T y{};

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

    vec2 operator+(vec2 &another) {
        return vec2{this->x + another.x, this->y + another.y};
    }

    vec2 operator-(vec2 &another) {
        return vec2{this->x - another.x, this->y - another.y};
    }

    vec2 operator*(vec2 &another) {
        return vec2{this->x * another.x, this->y * another.y};
    }

    vec2 operator/(vec2 &another) {
        return vec2{this->x / another.x, this->y / another.y};
    }

    vec2 &operator+=(vec2 &another) {
        this->x += another.x;
        this->y += another.y;
        return *this;
    }

    vec2 &operator-=(vec2 &another) {
        this->x -= another.x;
        this->y -= another.y;
        return *this;
    }

    vec2 &operator*=(vec2 &another) {
        this->x *= another.x;
        this->y *= another.y;
        return *this;
    }

    vec2 &operator/=(vec2 &another) {
        this->x /= another.x;
        this->y /= another.y;
        return *this;
    }

    bool operator==(const vec2 &another) const {
        return this->x == another.x && this->y == another.y;
    }

    bool operator!=(const vec2 &another) const {
        return !(this == another);
    }
};

#endif //ATCODERC_VECTOR_HPP
