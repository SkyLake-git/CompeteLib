#pragma once
#include <utility>

namespace record_cmp {
    struct less_pair_second {
        template<typename T1, typename T2>
        bool operator()(std::pair<T1, T2> &a, std::pair<T1, T2> &b) {
            return a.second < b.second;
        }
    };

    struct greater_pair_first_else {
        template<typename T1, typename T2>
        bool operator()(std::pair<T1, T2> &a, std::pair<T1, T2> &b) {
            if (a.first == b.first) {
                return a.second < b.second;
            }
            return a.first > b.first;
        }
    };

    struct greater_pair_second {
        template<typename T1, typename T2>
        bool operator()(std::pair<T1, T2> &a, std::pair<T1, T2> &b) {
            return a.second > b.second;
        }
    };
}
