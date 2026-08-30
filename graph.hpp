#ifndef ATCODERC_GRAPH_HPP
#define ATCODERC_GRAPH_HPP
#include <vector>
#include <algorithm>

#include "typings.hpp"

namespace graph {
    template<class T>
    struct edge {
        int from, to;
        T val;

        explicit edge(int f = -1, int t = -1, T v = -1) : from(f), to(t), val(v) {
        }
    };

    template<class T>
    struct cycle_detector {
        std::vector<std::vector<edge<T>>> G;

        std::vector<bool> seen, finished;
        std::vector<edge<T>> history;

        cycle_detector() = default;

        explicit cycle_detector(const std::vector<std::vector<edge<T>>> &graph) { init(graph); }

        void init(const std::vector<std::vector<edge<T>>> &graph) {
            G = graph;
            seen.assign(G.size(), false);
            finished.assign(G.size(), false);
        }

        int dfs(int v, const edge<T> &e, bool is_prohibit_reverse = true) {
            seen[v] = true;
            history.push_back(e);
            for (const edge<T> &e2: G[v]) {
                if (is_prohibit_reverse && e2.to == e.from) continue;

                if (finished[e2.to]) continue;

                if (seen[e2.to] && !finished[e2.to]) {
                    history.push_back(e2);
                    return e2.to;
                }

                int pos = this->dfs(e2.to, e2, is_prohibit_reverse);
                if (pos != -1) return pos;
            }
            finished[v] = true;
            history.pop_back();
            return -1;
        }

        std::vector<edge<T>> reconstruct(int pos) {
            std::vector<edge<T>> cycle;

            while (!history.empty()) {
                const edge<T> &e = history.back();
                cycle.push_back(e);
                history.pop_back();
                if (e.from == pos) break;
            }

            std::reverse(cycle.begin(), cycle.end());
            return cycle;
        }

        /**
         * @param is_prohibit_reverse false = directed
         * @return std::vector<edge<T>>
         */
        std::vector<edge<T>> detect(bool is_prohibit_reverse) {
            int pos = -1;
            for (int v = 0; v < static_cast<int>(G.size()) && pos == -1; ++v) {
                if (seen[v]) continue;
                history.clear();
                pos = this->dfs(v, edge<T>(), is_prohibit_reverse);
                if (pos != -1) return this->reconstruct(pos);
            }
            return std::vector<edge<T>>();
        }
    };
}

// https://drken1215.hatenablog.com/entry/2023/05/20/200517
namespace graph::functional {
    inline std::vector<int> detect_cycle(const std::vector<int> &graph) {
        int v = 0;
        for (int i = 0; i < static_cast<int>(graph.size()); ++i) v = graph[v];

        std::vector<int> res;
        int start = v;
        do {
            res.push_back(v);
            v = graph[v];
        } while (v != start);
        return res;
    }
}

namespace graph::directed {
    inline std::vector<edge<int>> detect_cycle(const std::vector<std::vector<edge<int>>> &graph) {
        cycle_detector detector(graph);
        return detector.detect(false);
    }
}

namespace graph::undirected {
    inline std::vector<edge<int>> detect_cycle(std::vector<std::vector<edge<int>>> &graph) {
        cycle_detector detector(graph);
        return detector.detect(true);
    }
}

#endif //ATCODERC_GRAPH_HPP
