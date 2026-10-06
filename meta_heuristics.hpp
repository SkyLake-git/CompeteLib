#ifndef ATCODERC_META_HEURISTICS_HPP
#define ATCODERC_META_HEURISTICS_HPP
#include <algorithm>
#include <unordered_set>

#include "debug.hpp"
#include "typings.hpp"
#include "utils.hpp"

template<class Operation>
struct abstract_sequential_state {
    using operation_type = Operation;
    virtual ~abstract_sequential_state() = default;
    virtual bool next(const Operation &) = 0;
    virtual std::generator<Operation> expand() const = 0;
    virtual long long calculate_next_score(const Operation &) const = 0;
    virtual bool is_terminal() const { return false; }
    virtual std::optional<unsigned long long> next_hash(const Operation &) const { return std::nullopt; }
};

template<Arithmetic T>
struct simulated_annealing {
    struct analytics {
        int iterations{};
        int accepted{};
        int rejected{};
        T score_balance{};
    };

    struct neighbor_data {
        int label{};
    };

    double temperature;
    double target_temperature;
    long long time_limit;
    stopwatch sa_stopwatch{};
    std::unordered_map<int, analytics> sa_analytics;
    bool analytics_enabled;


    explicit simulated_annealing(double initial_temperature, double target_temperature, long long time_limit) {
        temperature = initial_temperature;
        this->target_temperature = target_temperature;
        this->time_limit = time_limit;
        this->analytics_enabled = !IS_ONLINE_JUDGE;
    }

    bool timeout() const {
        return sa_stopwatch.elapsed_ms() >= time_limit;
    }

    bool should_accept(T score, T new_score, const neighbor_data &data) {
        double slope = static_cast<double>(sa_stopwatch.elapsed_ms()) / time_limit;

        double current_temperature = temperature + (target_temperature - temperature) * slope;

        T diff = new_score - score;
        if (diff > 0) {
            return true;
        }

        bool res = exp(static_cast<double>(diff) / current_temperature) > randf();

        if (analytics_enabled) {
            ++sa_analytics[data.label].iterations;
            if (res) {
                sa_analytics[data.label].score_balance += diff;
                ++sa_analytics[data.label].accepted;
                return true;
            }
            ++sa_analytics[data.label].rejected;
            return false;
        }

        return res;
    }

    bool should_accept(T score, T new_score) {
        return should_accept(score, new_score, neighbor_data{-1});
    }

    void print_analytics() const {
        if (!analytics_enabled) {
            std::cerr << "simulated_annealing: analytics disabled" << "\n";
            return;
        }
        analytics aggregated_analytics;

        std::string iterations_breakdown;
        for (const analytics &label_analytics: sa_analytics | std::views::values) {
            aggregated_analytics.iterations += label_analytics.iterations;
            aggregated_analytics.accepted += label_analytics.accepted;
            aggregated_analytics.rejected += label_analytics.rejected;
            aggregated_analytics.score_balance += label_analytics.score_balance;
        }

        for (auto &[label, label_analytics]: sa_analytics) {
            iterations_breakdown += format_patched(
                "- {:<4}: iterations {:>5} ({:>5}/{:<5}), accepted {:>5} ({:>5}/{:<5}), rejected {:>5} ({:>5}/{:<5}), score balance: {}\n",
                label,
                format_percentage(static_cast<double>(label_analytics.iterations) / aggregated_analytics.iterations),
                label_analytics.iterations,
                aggregated_analytics.iterations,
                format_percentage(static_cast<double>(label_analytics.accepted) / aggregated_analytics.accepted),
                label_analytics.accepted,
                aggregated_analytics.accepted,
                format_percentage(static_cast<double>(label_analytics.rejected) / aggregated_analytics.rejected),
                label_analytics.rejected,
                aggregated_analytics.rejected,
                label_analytics.score_balance
            );
        }


        std::cerr << "-- simulated_annealing: analytics --" << "\n";
        std::cerr << "iterations   : " << aggregated_analytics.iterations << "\n";
        std::cerr << "accepted     : " << aggregated_analytics.accepted << "\n";
        std::cerr << "rejected     : " << aggregated_analytics.rejected << "\n";
        std::cerr << "score balance: " << aggregated_analytics.score_balance << "\n";
        std::cerr << iterations_breakdown << "\n";
    }
};

template<class T>
concept SequentialState =
        requires
        {
            typename T::operation_type;
        } &&
        std::derived_from<T, abstract_sequential_state<typename T::operation_type>>;

template<SequentialState T>
struct beam_search {
    struct analytics {
        int iterations;
        int accepted;
        int discarded;
        int total_neighbors;
    };

    using operation_type = T::operation_type;

protected:
    struct candidate {
        long long score{};
        std::size_t state_index{};
        operation_type operation;
        std::optional<unsigned long long> hash;
    };

public:
    int beam_width{};
    std::vector<T> beam{};
    std::vector<T> terminals{};
    analytics bs_analytics{};
    bool analytics_enabled{};

    explicit beam_search(T origin_state, int beam_width) : beam_width(beam_width) {
        beam.push_back(std::move(origin_state));
        analytics_enabled = !IS_ONLINE_JUDGE;
    }

    bool step() {
        std::vector<candidate> next_beam;
        for (int i = 0; i < static_cast<int>(beam.size()); ++i) {
            for (auto &&next_op: beam[i].expand()) {
                const long long score = beam[i].calculate_next_score(next_op);
                const std::optional<unsigned long long> hash = beam[i].next_hash(next_op);
                next_beam.emplace_back(score, static_cast<std::size_t>(i), std::move(next_op), hash);
            }
        }
        if (next_beam.empty()) {
            return false;
        }

        std::ranges::stable_sort(next_beam, [](const candidate &a, const candidate &b) {
            return a.score > b.score;
        });

        std::vector<T> composited_beam;
        std::unordered_set<unsigned long long> seen;
        for (const candidate &c: next_beam) {
            if (static_cast<int>(composited_beam.size()) >= beam_width) {
                break;
            }
            if (c.hash.has_value() && !seen.insert(*c.hash).second) {
                continue;
            }
            T new_state = beam[c.state_index];
            if (!new_state.next(c.operation)) {
                continue;
            }
            composited_beam.push_back(std::move(new_state));
            if (analytics_enabled) {
                ++bs_analytics.accepted;
            }
            if (new_state.is_terminal()) {
                terminals.push_back(std::move(new_state));
                continue;
            }
            composited_beam.push_back(std::move(new_state));
        }

        if (analytics_enabled) {
            ++bs_analytics.iterations;
            bs_analytics.discarded += static_cast<int>(next_beam.size()) - static_cast<int>(composited_beam.size());
        }

        if (composited_beam.empty()) {
            return false;
        }
        beam = std::move(composited_beam);
        return true;
    }

    std::pair<T, bool> next() {
        const bool advanced = step();
        return {beam.front(), advanced};
    }
};

#endif //ATCODERC_META_HEURISTICS_HPP
