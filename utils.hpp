#ifndef ATCODERC_UTILS_HPP
#define ATCODERC_UTILS_HPP

#include <ios>
#include <iostream>
#include <random>

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
}

inline unsigned long long randl_range(unsigned long long min_val, unsigned long long max_val) {
    std::uniform_int_distribution get_rand_uni_int(min_val, max_val);
    return get_rand_uni_int(rnd_mt64);
}

inline long double randf() {
    return cast(randl_range(0, std::numeric_limits<unsigned long long>::max()),
                long double) / std::numeric_limits<unsigned long
               long>::max();
}

inline void hack_syncio() {
#ifdef ONLINE_JUDGE
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
#endif
}

#endif //ATCODERC_UTILS_HPP
