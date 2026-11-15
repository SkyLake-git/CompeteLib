#pragma once

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
void gp_slide(T n, Validator &&is_ok, Applier &&apply) {
    T right = 0;

    for (T left = 0; left < n; ++left) {
        if (right < left) {
            right = left;
        }

        while (right < n && is_ok(left, right)) {
            ++right;
        }

        apply(left, right);
    }
}

/**
 * 汎用尺取り法 + 区間和管理 O(n)
 * @param n
 * @param is_ok is_ok(l, r): r を追加しても条件を満たすかどうか
 * @param apply apply(l, r): 区間 l, r を答えに反映させる
 * @param add_right add_right(r): r を区間和に追加する
 * @param sub_left sub_left(l): l を区間和から引く
 */
template<Arithmetic T, Callable<T, T> Validator, Callable<T, T> Applier, Callable<T> Addition, Callable<T> Subtraction>
void gp_slide(T n, Validator &&is_ok, Applier &&apply, Addition &&add_right, Subtraction &&sub_left) {
    T right = 0;

    for (T left = 0; left < n; ++left) {
        while (right < n && is_ok(left, right)) {
            add_right(right);
            ++right;
        }

        apply(left, right);

        if (right == left) {
            ++right;
        } else {
            sub_left(left);
        }
    }
}
