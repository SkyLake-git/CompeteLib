#ifndef ATCODERC_UTILS_HPP
#define ATCODERC_UTILS_HPP

#include <ios>
#include <iostream>
#include <random>
#include <cstdlib>
#include <generator>

#define No cout << "No" << endl
#define Yes cout << "Yes" << endl
#define zerofill(obj) memset(obj, 0, sizeof(obj))
#define nonefill(obj) memset(obj, -1, sizeof(obj))
#define cast(obj, t) static_cast<t>(obj)

static std::mt19937_64 rnd_mt64(100);

struct stopwatch {
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    stopwatch() = default;

    [[nodiscard]] long long elapsed_ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).
                count();
    }

    void reset() {
        start_time = std::chrono::steady_clock::now();
    }
};

namespace timer {
    static stopwatch _internal_stopwatch{};
    static long long _limit{};

    inline void start(long long limit) {
        _limit = limit;
        _internal_stopwatch.reset();
    }

    inline bool is_timeout() {
        return _internal_stopwatch.elapsed_ms() >= _limit;
    }

    inline void reset() {
        return _internal_stopwatch.reset();
    }
}

/**
 * [min_val, max_val]
 */
inline unsigned long long randl_range(unsigned long long min_val, unsigned long long max_val) {
    std::uniform_int_distribution get_rand_uni_int(min_val, max_val);
    return get_rand_uni_int(rnd_mt64);
}

/**
 * [min_val, max_val]
 */
inline unsigned int randi_range(unsigned int min_val, unsigned int max_val) {
    std::uniform_int_distribution get_rand_uni_int(min_val, max_val);
    return get_rand_uni_int(rnd_mt64);
}

inline long double randf() {
    return cast(randl_range(0, std::numeric_limits<unsigned long long>::max()),
                long double) / std::numeric_limits<unsigned long
               long>::max();
}

inline std::generator<int> range_bfs(int min, int max, int start) {
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
inline std::generator<int> range_mid_bfs(int min, int max) {
    return range_bfs(min, max, (min + max) / 2);
}

template<typename T>
std::ostream &operator<<(std::ostream &os, std::vector<T> arr) {
    std::string s = "[ ";
    for (int i = 0; i < arr.size(); ++i) {
        s.append(std::to_string(arr[i]) + ", ");
    }

    os << s << "]";
    return os;
}

// bypass clangd lifehack
#ifdef ONLINE_JUDGE
std::string format_patched(
    const std::string &fmt,
    const auto &... args) {
    return std::vformat(fmt, std::make_format_args(args...));
}
#else
#include <fmt/format.h>

std::string format_patched(
    const std::string &fmt,
    const auto &... args) {
    return fmt::format(fmt::runtime(fmt), args...);
}
#endif

inline std::string format_percentage(double value) {
    return format_patched("{:.1f}%", value * 100);
}

inline int getenv_int(const char *name, const int fallback) {
    const char *val = getenv(name);
    if (val == nullptr) {
        return fallback;
    }
    return std::stoi(val);
}

inline long long getenv_ll(const char *name, const long long fallback) {
    const char *val = getenv(name);
    if (val == nullptr) {
        return fallback;
    }
    return std::stoll(val);
}

inline double getenv_double(const char *name, const double fallback) {
    const char *val = getenv(name);
    if (val == nullptr) {
        return fallback;
    }
    return std::stod(val);
}

#define hyperparameter_int(val_name, env_name, fallback) const static int val_name = getenv_int(env_name, fallback)
#define hyperparameter_ll(val_name, env_name, fallback) const static long long val_name = getenv_ll(env_name, fallback)
#define hyperparameter_double(val_name, env_name, fallback) const static double val_name = getenv_double(env_name, fallback)

#endif //ATCODERC_UTILS_HPP
