#ifndef ATCODERC_VECTOR_HPP
#define ATCODERC_VECTOR_HPP
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "lib/typings.hpp"
#include "lib/utils.hpp"

template<Arithmetic T>
struct vec2 {
    T x{};
    T y{};

    vec2() = default;

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

    vec2 operator+(const vec2 &another) const {
        return vec2{this->x + another.x, this->y + another.y};
    }

    vec2 operator-(const vec2 &another) const {
        return vec2{this->x - another.x, this->y - another.y};
    }

    vec2 operator*(const vec2 &another) const {
        return vec2{this->x * another.x, this->y * another.y};
    }

    vec2 operator/(const vec2 &another) const {
        return vec2{this->x / another.x, this->y / another.y};
    }

    vec2 &operator+=(const vec2 &another) const {
        this->x += another.x;
        this->y += another.y;
        return *this;
    }

    vec2 &operator-=(const vec2 &another) const {
        this->x -= another.x;
        this->y -= another.y;
        return *this;
    }

    vec2 &operator*=(const vec2 &another) const {
        this->x *= another.x;
        this->y *= another.y;
        return *this;
    }

    vec2 &operator/=(const vec2 &another) const {
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

struct vec2i : vec2<int> {
};

struct vec2l : vec2<long long> {
};

template<DistanceAlgo C, Arithmetic T, Arithmetic R>
R calc_vec_distance(const vec2<T> &a, const vec2<T> &b) {
    if constexpr (C == Euclidean) {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
    } else if constexpr (C == Manhattan) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    } else {
        throw std::logic_error("");
    }
}

template<DistanceAlgo C, Arithmetic T, Arithmetic R>
bool eval_vec_distance(const vec2<T> &a, const vec2<T> &b, R range) {
    if constexpr (C == Euclidean) {
        return calc_vec_distance<C, T, R>(a, b) <= range * range;
    } else {
        return calc_vec_distance<C, T, R>(a, b) <= range;
    }
}

/**
 * 指定範囲の二次元空間にある大量の点に対して、以下の操作を高速に行える構造体
 * - 指定位置から一定距離内の点を全て列挙
 * - 指定位置から最も近い点を取得
 *
 * 点が広く分布している・範囲がそれなりに広い・点の数がそれなりに多い 場合に有効です。
 * それ以外の場合は、全探索の方が高速なことに注意してください。
 *
 * @tparam T @see {vec2}
 * @tparam X x の最大範囲 [-x, x)
 * @tparam Y y の最大範囲 [-y, y)
 * @tparam BITS チャンク内の相対座標のビット数 = チャンクのサイズ (Ex. BITS=4, Coordinate Size=16 (2^4))
 */
template<Arithmetic T, unsigned short X, unsigned short Y, unsigned short BITS>
struct bounded_plane {
protected:
    unsigned int next_id = 0;

    std::vector<unsigned int>
    chunks[(((X - 1) >> BITS) + 1) * 2]
    [(((Y - 1) >> BITS) + 1) * 2];

    std::vector<vec2<T>> points;

    unsigned short X_CHUNK_HALF = (((X - 1) >> BITS) + 1);
    unsigned short Y_CHUNK_HALF = (((Y - 1) >> BITS) + 1);

public:
    explicit bounded_plane() : chunks() {
    }

    std::pair<unsigned int, unsigned int> get_chunk_count() const {
        return {
            X_CHUNK_HALF * 2,
            Y_CHUNK_HALF * 2
        };
    }

    unsigned int get_chunk_size() const {
        return 1 << BITS;
    }

    std::pair<unsigned int, unsigned int>
    get_chunk_indexes(const int &x, const int &y) const {
        short fx = static_cast<short>(
            std::clamp(x, -X, X + 1)
        );

        short fy = static_cast<short>(
            std::clamp(y, -Y, Y + 1)
        );

        return {
            X_CHUNK_HALF + (fx >> BITS),
            Y_CHUNK_HALF + (fy >> BITS)
        };
    }

    vec2<T> &get_point(unsigned int index) {
        return points[index];
    }

    unsigned int add_point(const vec2<T> &v) {
        if (v.x <= -X || v.x > X ||
            v.y <= -Y || v.y > Y) {
            throw std::runtime_error("Out of bounds");
        }

        unsigned int id = next_id++;

        auto [cx, cy] =
                get_chunk_indexes(v.x, v.y);

        chunks[cx][cy].push_back(id);
        points.push_back(v);

        return id;
    }

    template<DistanceAlgo C, Arithmetic R>
    R calc_chunk_distance(
        const vec2<T> &position,
        unsigned int cx,
        unsigned int cy
    ) const {
        const R chunk_min_x =
                static_cast<R>(
                    (static_cast<int>(cx) - X_CHUNK_HALF) << BITS
                );

        const R chunk_max_x =
                chunk_min_x + (1 << BITS) - 1;

        const R chunk_min_y =
                static_cast<R>(
                    (static_cast<int>(cy) - Y_CHUNK_HALF) << BITS
                );

        const R chunk_max_y =
                chunk_min_y + (1 << BITS) - 1;

        R dx = 0;
        R dy = 0;

        if (position.x < chunk_min_x) {
            dx = chunk_min_x - static_cast<R>(position.x);
        } else if (position.x > chunk_max_x) {
            dx = static_cast<R>(position.x) - chunk_max_x;
        }

        if (position.y < chunk_min_y) {
            dy = chunk_min_y - static_cast<R>(position.y);
        } else if (position.y > chunk_max_y) {
            dy = static_cast<R>(position.y) - chunk_max_y;
        }

        if constexpr (C == Euclidean) {
            return dx * dx + dy * dy;
        } else if constexpr (C == Manhattan) {
            return dx + dy;
        } else {
            static_assert(
                C == Euclidean || C == Manhattan,
                "Unsupported distance algorithm"
            );
        }
    }

    template<DistanceAlgo C, Arithmetic R>
    bool eval_chunk_distance(
        const vec2<T> &position,
        unsigned int cx,
        unsigned int cy,
        R range
    ) const {
        return calc_chunk_distance<C, R>(
                   position,
                   cx,
                   cy
               ) <= range;
    }

    template<DistanceAlgo C, Arithmetic R>
    std::vector<unsigned int>
    query_range(
        const vec2<T> &position,
        R range
    ) {
        auto [min_cx, min_cy] =
                get_chunk_indexes(
                    position.x - range,
                    position.y - range
                );

        auto [max_cx, max_cy] =
                get_chunk_indexes(
                    position.x + range,
                    position.y + range
                );

        std::vector<unsigned int> res;

        for (unsigned int cx = min_cx;
             cx <= max_cx;
             ++cx) {
            for (unsigned int cy = min_cy;
                 cy <= max_cy;
                 ++cy) {
                if (chunks[cx][cy].empty()) {
                    continue;
                }

                if (!eval_chunk_distance<C>(
                    position,
                    cx,
                    cy,
                    range
                )) {
                    continue;
                }

                for (unsigned int p: chunks[cx][cy]) {
                    if (!eval_vec_distance<C>(
                        position,
                        points[p],
                        range
                    )) {
                        continue;
                    }

                    res.push_back(p);
                }
            }
        }

        return res;
    }

    template<DistanceAlgo C, Arithmetic R>
    std::vector<unsigned int>
    query_range_nearest(
        const vec2<T> &position,
        R range
    ) {
        unsigned int max_cx;
        unsigned int max_cy;

        if (range > X && range > Y) {
            max_cx = X_CHUNK_HALF * 2 - 1;
            max_cy = Y_CHUNK_HALF * 2 - 1;
        } else {
            auto [cx, cy] =
                    get_chunk_indexes(
                        position.x + range,
                        position.y + range
                    );

            max_cx = cx;
            max_cy = cy;
        }

        const auto [mid_cx, mid_cy] =
                get_chunk_indexes(
                    position.x,
                    position.y
                );

        R min_dist = range;

        std::vector<unsigned int> res;

        const unsigned int max_radius =
                std::max({
                    mid_cx,
                    mid_cy,
                    max_cx - mid_cx,
                    max_cy - mid_cy
                });

        auto check_chunk =
                [&](unsigned int cx, unsigned int cy) {
            if (chunks[cx][cy].empty()) {
                return;
            }

            if (!eval_chunk_distance<C>(
                position,
                cx,
                cy,
                min_dist
            )) {
                return;
            }

            for (unsigned int p: chunks[cx][cy]) {
                R dist =
                        calc_vec_distance<C, T, R>(
                            position,
                            points[p]
                        );

                if (dist < min_dist) {
                    min_dist = dist;
                    res.clear();
                    res.push_back(p);
                } else if (dist == min_dist) {
                    res.push_back(p);
                }
            }
        };

        for (unsigned int radius = 0;
             radius <= max_radius;
             ++radius) {
            unsigned int min_cx =
                    mid_cx > radius
                        ? mid_cx - radius
                        : 0;

            unsigned int max_x =
                    std::min(
                        mid_cx + radius,
                        max_cx
                    );

            unsigned int min_cy =
                    mid_cy > radius
                        ? mid_cy - radius
                        : 0;

            unsigned int max_y =
                    std::min(
                        mid_cy + radius,
                        max_cy
                    );

            // 中心
            if (radius == 0) {
                check_chunk(mid_cx, mid_cy);
                continue;
            }

            // 上辺・下辺
            for (unsigned int cx = min_cx;
                 cx <= max_x;
                 ++cx) {
                check_chunk(cx, min_cy);

                if (max_y != min_cy) {
                    check_chunk(cx, max_y);
                }
            }

            // 左辺・右辺
            for (unsigned int cy = min_cy + 1;
                 cy < max_y;
                 ++cy) {
                check_chunk(min_cx, cy);

                if (max_x != min_cx) {
                    check_chunk(max_x, cy);
                }
            }

            /*
             * 現在の最短距離より外側にある
             * チャンクだけになったら終了できる。
             *
             * 次の radius の最小チャンク距離を調べる。
             */
            if (radius < max_radius) {
                const unsigned int next_radius =
                        radius + 1;

                const unsigned int next_min_cx =
                        mid_cx > next_radius
                            ? mid_cx - next_radius
                            : 0;

                const unsigned int next_max_cx =
                        std::min(
                            mid_cx + next_radius,
                            max_cx
                        );

                const unsigned int next_min_cy =
                        mid_cy > next_radius
                            ? mid_cy - next_radius
                            : 0;

                const unsigned int next_max_cy =
                        std::min(
                            mid_cy + next_radius,
                            max_cy
                        );

                R next_min_chunk_dist =
                        std::numeric_limits<R>::max();

                auto update_min_chunk_distance =
                        [&](unsigned int cx,
                            unsigned int cy) {
                    if (chunks[cx][cy].empty()) {
                        return;
                    }

                    R d =
                            calc_chunk_distance<C, R>(
                                position,
                                cx,
                                cy
                            );

                    next_min_chunk_dist =
                            std::min(
                                next_min_chunk_dist,
                                d
                            );
                };

                for (unsigned int cx = next_min_cx;
                     cx <= next_max_cx;
                     ++cx) {
                    update_min_chunk_distance(
                        cx,
                        next_min_cy
                    );

                    if (next_max_cy != next_min_cy) {
                        update_min_chunk_distance(
                            cx,
                            next_max_cy
                        );
                    }
                }

                for (unsigned int cy = next_min_cy + 1;
                     cy < next_max_cy;
                     ++cy) {
                    update_min_chunk_distance(
                        next_min_cx,
                        cy
                    );

                    if (next_max_cx != next_min_cx) {
                        update_min_chunk_distance(
                            next_max_cx,
                            cy
                        );
                    }
                }

                if (next_min_chunk_dist >= min_dist) {
                    break;
                }
            }
        }

        return res;
    }

    template<DistanceAlgo C, Arithmetic R>
    std::optional<unsigned int>
    get_range_nearest(
        const vec2<T> &position,
        R range
    ) {
        unsigned int max_cx;
        unsigned int max_cy;

        if (range == std::numeric_limits<R>::max()) {
            max_cx = X_CHUNK_HALF * 2 - 1;
            max_cy = Y_CHUNK_HALF * 2 - 1;
        } else {
            auto [cx, cy] =
                    get_chunk_indexes(
                        position.x + range,
                        position.y + range
                    );

            max_cx = cx;
            max_cy = cy;
        }

        const auto [mid_cx, mid_cy] =
                get_chunk_indexes(
                    position.x,
                    position.y
                );

        R min_dist = range;
        unsigned int res = next_id;

        const unsigned int max_radius =
                std::max({
                    mid_cx,
                    mid_cy,
                    max_cx - mid_cx,
                    max_cy - mid_cy
                });

        auto check_chunk =
                [&](unsigned int cx, unsigned int cy) {
            const auto &chunk = chunks[cx][cy];

            if (chunk.empty()) {
                return;
            }

            /*
             * get_nearest() なので、
             * chunk_dist == min_dist の場合も
             * 新しい nearest は存在しない。
             */
            const R chunk_dist =
                    calc_chunk_distance<C, R>(
                        position,
                        cx,
                        cy
                    );

            if (chunk_dist >= min_dist) {
                return;
            }

            for (unsigned int p: chunks[cx][cy]) {
                const R dist =
                        calc_vec_distance<C, T, R>(
                            position,
                            points[p]
                        );

                if (dist < min_dist ||
                    (dist == min_dist && p < res)) {
                    min_dist = dist;
                    res = p;
                }
            }
        };

        /*
         * radius ごとの正方形全体ではなく、
         * 「外周」だけを走査する。
         *
         * これによって同じチャンクを何度も
         * 訪問することを防ぐ。
         */
        for (unsigned int radius = 0;
             radius <= max_radius;
             ++radius) {
            const unsigned int min_cx =
                    mid_cx > radius
                        ? mid_cx - radius
                        : 0;

            const unsigned int max_x =
                    std::min(
                        mid_cx + radius,
                        max_cx
                    );

            const unsigned int min_cy =
                    mid_cy > radius
                        ? mid_cy - radius
                        : 0;

            const unsigned int max_y =
                    std::min(
                        mid_cy + radius,
                        max_cy
                    );

            /*
             * radius == 0
             *
             * 中心チャンクのみ。
             */
            if (radius == 0) {
                check_chunk(mid_cx, mid_cy);
                continue;
            }

            /*
             * 上辺・下辺
             */
            for (unsigned int cx = min_cx;
                 cx <= max_x;
                 ++cx) {
                check_chunk(cx, min_cy);

                if (max_y != min_cy) {
                    check_chunk(cx, max_y);
                }
            }

            /*
             * 左辺・右辺
             *
             * 角は上辺・下辺ですでに処理済みなので、
             * min_cy + 1 ～ max_y - 1 のみ。
             */
            for (unsigned int cy = min_cy + 1;
                 cy < max_y;
                 ++cy) {
                check_chunk(min_cx, cy);

                if (max_x != min_cx) {
                    check_chunk(max_x, cy);
                }
            }
        }

        if (res == next_id) {
            return std::nullopt;
        }

        return res;
    }

    template<DistanceAlgo C>
    std::vector<unsigned int> query_nearest(const vec2<T> &position) {
        using R = long long;

        return query_range_nearest<C, R>(
            position,
            std::numeric_limits<R>::max()
        );
    }

    template<DistanceAlgo C>
    std::optional<unsigned int> get_nearest(const vec2<T> &position) {
        using R = long long;

        return get_range_nearest<C, R>(
            position,
            std::numeric_limits<R>::max()
        );
    }
};

#endif //ATCODERC_VECTOR_HPP
