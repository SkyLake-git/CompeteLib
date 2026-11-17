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
void sliding_window(T n, Validator &&is_ok, Applier &&apply) {
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
void sliding_window(T n, Validator &&is_ok, Applier &&apply, Addition &&add_right, Subtraction &&sub_left) {
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

/**
 * 二分探索 O(log n)
 *
 * @param ok 条件を満たすと分かっている値
 * @param ng 条件を満たさないと分かっている値
 * @param is_ok is_ok(mid): mid が条件を満たすかどうか
 * @return {ok: 条件を満たす値の中で最大の値, ng: 条件を満たさない値の中で最小の値}
 */
template<Arithmetic T, Callable<T> Validator>
std::pair<T, T> bisect(T ok, T ng, Validator &&is_ok) {
    while (std::abs(ok - ng) > 1) {
        T mid = std::midpoint(ok, ng); // オーバーフローが起こりにくいらしい

        if (is_ok(mid)) {
            ok = mid;
        } else {
            ng = mid;
        }
    }

    return {ok, ng};
}

/**
 * 元の変数を変更する二分探索 O(log n)
 *
 * @param ok 条件を満たすと分かっている値
 * @param ng 条件を満たさないと分かっている値
 * @param is_ok is_ok(mid): mid が条件を満たすかどうか
 */
template<Arithmetic T, Callable<T> Validator>
void bisect(T &ok, T &ng, Validator &&is_ok) {
    while (std::abs(ok - ng) > 1) {
        T mid = std::midpoint(ok, ng); // オーバーフローが起こりにくいらしい

        if (is_ok(mid)) {
            ok = mid;
        } else {
            ng = mid;
        }
    }
}
