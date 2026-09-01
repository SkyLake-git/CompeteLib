#ifndef ATCODERC_UTILS_HPP
#define ATCODERC_UTILS_HPP

#include <ios>
#include <iostream>
#include <random>
#include <generator>

#define No cout << "No" << endl
#define Yes cout << "Yes" << endl
#define zerofill(obj) memset(obj, 0, sizeof(obj))
#define nonefill(obj) memset(obj, -1, sizeof(obj))
#define cast(obj, t) static_cast<t>(obj)

static std::mt19937_64 rnd_mt64(100);

namespace timer {
    static std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    inline long long elapsed_ms() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).
                count();
    }

    inline void reset() {
        start_time = std::chrono::steady_clock::now();
    }
}

inline unsigned long long randl_range(unsigned long long min_val, unsigned long long max_val) {
    std::uniform_int_distribution get_rand_uni_int(min_val, max_val);
    return get_rand_uni_int(rnd_mt64);
}

inline unsigned int randi_range(unsigned int min_val, unsigned int max_val) {
    std::uniform_int_distribution get_rand_uni_int(min_val, max_val);
    return get_rand_uni_int(rnd_mt64);
}

inline long double randf() {
    return cast(randl_range(0, std::numeric_limits<unsigned long long>::max()),
                long double) / std::numeric_limits<unsigned long
               long>::max();
}

inline std::generator<int> range_dfs(int min, int max, int start) {
    co_yield start;
    for (int d = 1; d <= max - min; ++d) {
        int right = start + d;
        int left = start - d;

        if (right <= max)
            co_yield right;

        if (left >= min)
            co_yield left;
    }
}

// (min + max) / 2 から1ずつmin, maxに近づいていく数列を返す
inline std::generator<int> range_mid_dfs(int min, int max) {
    return range_dfs(min, max, (min + max) / 2);
}

inline void hack_syncio() {
#ifdef ONLINE_JUDGE
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
#endif
}

#endif //ATCODERC_UTILS_HPP
