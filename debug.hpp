#pragma once
#include <iostream>

#ifndef ONLINE_JUDGE
#define printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define printf(...) NOP_LOCAL_DEBUG(__VA_ARGS__)
#endif

// ローカルデバッグ時に printf に渡された内容
void NOP_LOCAL_DEBUG(const auto &... args) {
}

template<Arithmetic T>
void debug_arr(auto arr, const T size) {
#ifndef ONLINE_JUDGE
    std::cerr << "[ ";
    for (int i = 0; i < size; ++i) {
        std::cerr << arr[i] << ", ";
    }

    std::cerr << "]" << std::endl;
#endif
}

#ifdef ONLINE_JUDGE
constexpr bool IS_ONLINE_JUDGE = true;
#else
constexpr bool IS_ONLINE_JUDGE = false;
#endif
