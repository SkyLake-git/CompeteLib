#ifndef ATCODERC_ALGORITHM_HPP
#define ATCODERC_ALGORITHM_HPP

#include <algorithm>
#include <cstddef>
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

#endif //ATCODERC_ALGORITHM_HPP
