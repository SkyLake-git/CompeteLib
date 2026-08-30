#ifndef ATCODERC_RECORDS_HPP
#define ATCODERC_RECORDS_HPP

template<typename T1, typename T2, typename T3>
struct triplet {
    T1 first{};
    T2 second{};
    T3 third{};

    triplet() = default;

    triplet(T1 first, T2 second, T3 third) {
        this->first = first;
        this->second = second;
        this->third = third;
    }

    auto operator<=>(const triplet &) const = default;

    struct less_second {
        bool operator()(triplet &a, triplet &b) {
            return a.second < b.second;
        }
    };

    struct greater_second {
        bool operator()(triplet &a, triplet &b) {
            return a.second > b.second;
        }
    };

    struct less_third {
        bool operator()(triplet &a, triplet &b) {
            return a.third < b.third;
        }
    };

    struct greater_third {
        bool operator()(triplet &a, triplet &b) {
            return a.third > b.third;
        }
    };
};

template<typename T1, typename T2, typename T3, typename T4>
struct quadruplet {
    T1 first{};
    T2 second{};
    T3 third{};
    T4 fourth{};

    quadruplet() = default;

    quadruplet(T1 first, T2 second, T3 third, T4 fourth) {
        this->first = first;
        this->second = second;
        this->third = third;
        this->fourth = fourth;
    }

    auto operator<=>(const quadruplet &) const = default;
};

#endif //ATCODERC_RECORDS_HPP
