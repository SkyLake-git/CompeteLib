#ifndef ATCODERC_DEBUG_HPP
#define ATCODERC_DEBUG_HPP
#include <iostream>

#ifndef ONLINE_JUDGE
#define printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define printf(...)
#endif

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
#endif //ATCODERC_DEBUG_HPP
