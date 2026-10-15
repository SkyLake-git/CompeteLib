#ifndef ATCODERC_ALGORITHM_HPP
#define ATCODERC_ALGORITHM_HPP

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

#include "typings.hpp"

/**
 * 汎用尺取り法 O(n)
 * @param n
 * @param is_ok is_ok(l, r): r を追加しても条件を満たすかどうか
 * @param apply apply(l, r): 区間 l, r を答えに反映させる
 */
template<Arithmetic T, Callable<T, T> Validator, Callable<T, T> Applier>
void gp_slide(T n, Validator is_ok, Applier apply) {
    T right = 0;

    for (T left = 0; left < n; ++left) {
        if (right < left) {
            right = left;
        }

        while (right < n && is_ok(left, right + 1)) {
            ++right;
        }

        apply(left, right);
    }
}

/**
 * 条件を満たす最長の半開区間 [left, right) を返す
 */
template<Arithmetic T, Callable<T, T> Validator>
std::pair<T, T> longest_valid_subarray(T n, Validator is_ok) {
    std::pair<T, T> result{0, 0};
    gp_slide(n, is_ok, [&result](T left, T right) {
        if (right - left > result.second - result.first) {
            result = {left, right};
        }
    });
    return result;
}

/**
 * 条件を満たす半開区間 [left, right) の個数を返す
 */
template<Arithmetic T, Callable<T, T> Validator>
T count_valid_subarrays(T n, Validator is_ok) {
    T result = 0;
    gp_slide(n, is_ok, [&result](T left, T right) {
        result += right - left;
    });
    return result;
}

/**
 * 条件を満たす最短の半開区間 [left, right) を返す
 */
template<Arithmetic T, Callable<T, T> Validator>
std::optional<std::pair<T, T>> shortest_valid_subarray(T n, Validator is_ok) {
    std::optional<std::pair<T, T>> result;
    T right = 0;

    for (T left = 0; left < n; ++left) {
        if (right < left) {
            right = left;
        }

        while (right < n && !is_ok(left, right + 1)) {
            ++right;
        }
        if (right == n) {
            break;
        }

        std::pair<T, T> candidate{left, right + 1};
        if (!result.has_value() || candidate.second - candidate.first < result->second - result->first) {
            result = candidate;
        }
    }

    return result;
}

#endif //ATCODERC_ALGORITHM_HPP
