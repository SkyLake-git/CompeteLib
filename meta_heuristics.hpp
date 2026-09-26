#ifndef ATCODERC_META_HEURISTICS_HPP
#define ATCODERC_META_HEURISTICS_HPP
#include <algorithm>

#include "typings.hpp"
#include "utils.hpp"

template<class Operation>
struct abstract_sequential_state {
    using operation_type = Operation;
    virtual ~abstract_sequential_state() = default;
    virtual bool next(const Operation &) = 0;
    virtual std::generator<Operation> expand() const = 0;
    virtual long long calculate_next_score(const Operation &) const = 0;
    virtual long long get_score() const = 0;
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
    using operation_type = T::operation_type;

protected:
    struct candidate {
        long long score;
        std::size_t state_index;
        operation_type operation;
    };

public:
    int beam_width{};
    std::vector<T> beam{};

    explicit beam_search(T origin_state, int beam_width) : beam_width(beam_width) {
        beam.push_back(origin_state);
    }

    std::pair<T, bool> next() {
        std::vector<candidate> next_beam;

        for (int i = 0; i < static_cast<int>(beam.size()); ++i) {
            const T &state = beam[i];
            for (auto next_op: state.expand()) {
                next_beam.emplace_back(state.calculate_next_score(next_op), i, next_op);
            }
        }

        if (next_beam.empty()) {
            return {beam.front(), false};
        }

        int effective_beam_width = std::min(beam_width, static_cast<int>(next_beam.size()));

        std::ranges::sort(next_beam, [](const auto &a, const auto &b) {
            return a.score > b.score;
        });
        std::vector<T> composited_beam;
        for (int i = 0; i < effective_beam_width; ++i) {
            T new_state = beam[next_beam[i].state_index];
            if (new_state.next(next_beam[i].operation)) {
                composited_beam.push_back(std::move(new_state));
            }
        }

        if (composited_beam.empty()) {
            return {beam.front(), false};
        }
        beam = std::move(composited_beam);

        return {beam.front(), true};
    }
};

#endif //ATCODERC_META_HEURISTICS_HPP
