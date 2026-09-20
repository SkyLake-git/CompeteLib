#ifndef ATCODERC_DATA_STRUCTURE_HPP
#define ATCODERC_DATA_STRUCTURE_HPP
#include <algorithm>
#include <utility>
#include "typings.hpp"
#include "vector.hpp"

/**
 * 指定範囲の二次元空間にある大量の点に対して、以下の操作を高速に行える構造体
 * - 指定位置から一定距離内の点を全て列挙
 * - 指定位置から一定距離内で最も近い点を取得
 * - 指定位置から最も近い点を取得
 *
 * 点が広く分布している・範囲がそれなりに広い・点の数がそれなりに多い 場合に有効です。
 * それ以外の場合は、全探索の方が高速な場合が多いことに注意してください。
 *
 * @authors SkyLake, Claude
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
    std::vector<unsigned int> chunks[(((X - 1) >> BITS) + 1) * 2][(((Y - 1) >> BITS) + 1) * 2];
    std::vector<vec2<T>> points;
    unsigned short X_CHUNK_HALF = (((X - 1) >> BITS) + 1);
    unsigned short Y_CHUNK_HALF = (((Y - 1) >> BITS) + 1);

public:
    explicit bounded_plane() : chunks() {
    };

    [[nodiscard]] std::pair<unsigned int, unsigned int> get_chunk_count() const {
        return {X_CHUNK_HALF * 2, Y_CHUNK_HALF * 2};
    }

    [[nodiscard]] unsigned int get_chunk_size() const {
        return 1 << BITS;
    }

    [[nodiscard]] std::pair<unsigned int, unsigned int> get_chunk_indexes(const int &x, const int &y) const {
        int fx = std::clamp(x, -static_cast<int>(X), static_cast<int>(X) + 1);
        int fy = std::clamp(y, -static_cast<int>(Y), static_cast<int>(Y) + 1);

        return {
            X_CHUNK_HALF + (fx >> BITS),
            Y_CHUNK_HALF + (fy >> BITS)
        };
    }

    vec2<T> &get_point(unsigned int index) {
        return points.at(index);
    }

    unsigned int add_point(const vec2<T> &v) {
        if (v.x <= -X || v.x > X || v.y <= -Y || v.y > Y) {
            throw std::runtime_error("Out of bounds");
        }
        unsigned int id = next_id++;
        auto [cx, cy] = get_chunk_indexes(v.x, v.y);
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

        const R chunk_max_x = chunk_min_x + (1 << BITS);

        const R chunk_min_y =
                static_cast<R>(
                    (static_cast<int>(cy) - Y_CHUNK_HALF) << BITS
                );

        const R chunk_max_y = chunk_min_y + (1 << BITS);

        R dx = 0;
        R dy = 0;

        if (position.x < chunk_min_x) {
            dx = chunk_min_x - position.x;
        } else if (position.x >= chunk_max_x) {
            dx = position.x - chunk_max_x;
        }

        if (position.y < chunk_min_y) {
            dy = chunk_min_y - position.y;
        } else if (position.y >= chunk_max_y) {
            dy = position.y - chunk_max_y;
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

            throw std::runtime_error("");
        }
    }

    template<DistanceAlgo C, Arithmetic R>
    bool eval_chunk_distance(const vec2<T> &position, unsigned int cx, unsigned int cy, R range) const {
        return calc_chunk_distance<C, R>(position, cx, cy) <= range;
    }

    template<DistanceAlgo C, Arithmetic R>
    std::vector<unsigned int> query_range(const vec2<T> &position, R range) {
        auto [min_cx, min_cy] = get_chunk_indexes(position.x - range, position.y - range);
        auto [max_cx, max_cy] = get_chunk_indexes(position.x + range, position.y + range);

        std::vector<unsigned int> res;

        for (unsigned int cx = min_cx; cx <= max_cx; ++cx) {
            for (unsigned int cy = min_cy; cy <= max_cy; ++cy) {
                for (unsigned int p: chunks[cx][cy]) {
                    if (!eval_vec_distance<C>(position, points[p], range)) {
                        continue;
                    }
                    res.push_back(p);
                }
            }
        }

        return res;
    }

private:
    template<Arithmetic R>
    R chunk_edge_min_x(int c) const { return static_cast<R>((c - static_cast<int>(X_CHUNK_HALF)) << BITS); }

    template<Arithmetic R>
    R chunk_edge_max_x(int c) const { return chunk_edge_min_x<R>(c) + (1 << BITS); }

    template<Arithmetic R>
    R chunk_edge_min_y(int c) const { return static_cast<R>((c - static_cast<int>(Y_CHUNK_HALF)) << BITS); }

    template<Arithmetic R>
    R chunk_edge_max_y(int c) const { return chunk_edge_min_y<R>(c) + (1 << BITS); }

    template<DistanceAlgo C, Arithmetic R, bool CollectTies, class Visit>
    void search_rings(const vec2<T> &position, int min_cx, int min_cy, int max_cx, int max_cy,
                      R initial_best, Visit &&on_candidate) {
        auto [mcx, mcy] = get_chunk_indexes(position.x, position.y);
        const int mid_cx = static_cast<int>(mcx);
        const int mid_cy = static_cast<int>(mcy);

        R best = initial_best;
        bool found = false;

        auto scan_cell = [&](int cx, int cy) {
            if (cx < min_cx || cx > max_cx || cy < min_cy || cy > max_cy) return;
            auto &bucket = chunks[cx][cy];
            if (bucket.empty()) return;
            if (!eval_chunk_distance<C>(position, static_cast<unsigned int>(cx), static_cast<unsigned int>(cy), best))
                return;
            for (unsigned int p: bucket) {
                R dist = calc_vec_distance<C, T, R>(position, points[p]);
                if (dist < best) {
                    best = dist;
                    found = true;
                    on_candidate(p, dist, true);
                } else if constexpr (CollectTies) {
                    if (found && dist == best) {
                        on_candidate(p, dist, false);
                    }
                }
            }
        };

        const int max_radius = std::max({mid_cx - min_cx, max_cx - mid_cx, mid_cy - min_cy, max_cy - mid_cy});

        for (int radius = 0; radius <= max_radius; ++radius) {
            if (radius == 0) {
                scan_cell(mid_cx, mid_cy);
            } else {
                const int top = mid_cy - radius, bottom = mid_cy + radius;
                const int left = mid_cx - radius, right = mid_cx + radius;
                for (int cx = left; cx <= right; ++cx) {
                    scan_cell(cx, top);
                    scan_cell(cx, bottom);
                }
                for (int cy = top + 1; cy <= bottom - 1; ++cy) {
                    scan_cell(left, cy);
                    scan_cell(right, cy);
                }
            }

            const bool left_open = (mid_cx - radius) > min_cx;
            const bool right_open = (mid_cx + radius) < max_cx;
            const bool top_open = (mid_cy - radius) > min_cy;
            const bool bottom_open = (mid_cy + radius) < max_cy;

            if (!left_open && !right_open && !top_open && !bottom_open) break;
            if (!found) continue; // area remains and we have no candidate yet - keep expanding

            bool have_gap = false;
            R best_gap = R{};
            auto consider = [&](R gap) {
                if (!have_gap || gap < best_gap) {
                    best_gap = gap;
                    have_gap = true;
                }
            };
            if (left_open) consider(static_cast<R>(position.x - chunk_edge_min_x<R>(mid_cx - radius)));
            if (right_open) consider(static_cast<R>(chunk_edge_max_x<R>(mid_cx + radius) - position.x));
            if (top_open) consider(static_cast<R>(position.y - chunk_edge_min_y<R>(mid_cy - radius)));
            if (bottom_open) consider(static_cast<R>(chunk_edge_max_y<R>(mid_cy + radius) - position.y));

            if (!have_gap) break;

            R gap_metric;
            if constexpr (C == Euclidean) gap_metric = best_gap * best_gap;
            else gap_metric = best_gap;

            if constexpr (CollectTies) {
                if (gap_metric > best) break;
            } else {
                if (gap_metric >= best) break;
            }
        }
    }

    template<DistanceAlgo C, Arithmetic R>
    void range_bounds(const vec2<T> &position, R range,
                      int &min_cx, int &min_cy, int &max_cx, int &max_cy, R &initial_best) const {
        if (range > X && range > Y) {
            min_cx = 0;
            min_cy = 0;
            max_cx = static_cast<int>(X_CHUNK_HALF) * 2 - 1;
            max_cy = static_cast<int>(Y_CHUNK_HALF) * 2 - 1;
        } else {
            auto [lo_cx, lo_cy] = get_chunk_indexes(
                static_cast<int>(position.x - range), static_cast<int>(position.y - range));
            auto [hi_cx, hi_cy] = get_chunk_indexes(
                static_cast<int>(position.x + range), static_cast<int>(position.y + range));
            min_cx = static_cast<int>(lo_cx);
            min_cy = static_cast<int>(lo_cy);
            max_cx = static_cast<int>(hi_cx);
            max_cy = static_cast<int>(hi_cy);
        }
        if constexpr (C == Euclidean) initial_best = range * range;
        else initial_best = range;
    }

public:
    template<DistanceAlgo C, Arithmetic R>
    std::vector<unsigned int> query_range_nearest(const vec2<T> &position, R range) {
        int min_cx, min_cy, max_cx, max_cy;
        R initial_best;
        range_bounds<C>(position, range, min_cx, min_cy, max_cx, max_cy, initial_best);

        std::vector<unsigned int> res;
        search_rings<C, R, true>(position, min_cx, min_cy, max_cx, max_cy, initial_best,
                                 [&](unsigned int p, R /*dist*/, bool is_new_best) {
                                     if (is_new_best) res.clear();
                                     res.push_back(p);
                                 });
        return res;
    }

    template<DistanceAlgo C, Arithmetic R>
    std::optional<unsigned int> get_range_nearest(const vec2<T> &position, R range) {
        int min_cx, min_cy, max_cx, max_cy;
        R initial_best;
        range_bounds<C>(position, range, min_cx, min_cy, max_cx, max_cy, initial_best);

        unsigned int res = next_id;
        search_rings<C, R, false>(position, min_cx, min_cy, max_cx, max_cy, initial_best,
                                  [&](unsigned int p, R /*dist*/, bool /*is_new_best*/) {
                                      res = p;
                                  });
        if (res == next_id) {
            return std::nullopt;
        }
        return res;
    }

    template<DistanceAlgo C>
    std::vector<unsigned int> query_nearest(const vec2<T> &position) {
        using R = std::conditional_t<std::is_floating_point_v<T>, long double, long long>;
        const int max_cx = static_cast<int>(X_CHUNK_HALF) * 2 - 1;
        const int max_cy = static_cast<int>(Y_CHUNK_HALF) * 2 - 1;

        std::vector<unsigned int> res;
        search_rings<C, R, true>(position, 0, 0, max_cx, max_cy, std::numeric_limits<R>::max(),
                                 [&](unsigned int p, R /*dist*/, bool is_new_best) {
                                     if (is_new_best) res.clear();
                                     res.push_back(p);
                                 });
        return res;
    }

    template<DistanceAlgo C>
    std::optional<unsigned int> get_nearest(const vec2<T> &position) {
        using R = std::conditional_t<std::is_floating_point_v<T>, long double, long long>;
        const int max_cx = static_cast<int>(X_CHUNK_HALF) * 2 - 1;
        const int max_cy = static_cast<int>(Y_CHUNK_HALF) * 2 - 1;

        unsigned int res = next_id;
        search_rings<C, R, false>(position, 0, 0, max_cx, max_cy, std::numeric_limits<R>::max(),
                                  [&](unsigned int p, R /*dist*/, bool /*is_new_best*/) {
                                      res = p;
                                  });
        if (res == next_id) {
            return std::nullopt;
        }
        return res;
    }
};


#endif //ATCODERC_DATA_STRUCTURE_HPP
