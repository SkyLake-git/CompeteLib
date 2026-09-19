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

struct simulated_annealing {
    double temperature;
    double target_temperature;
    long long time_limit;
    stopwatch sa_stopwatch{};

    explicit simulated_annealing(double initial_temperature, double target_temperature, long long time_limit) {
        temperature = initial_temperature;
        this->target_temperature = target_temperature;
        this->time_limit = time_limit;
    }

    bool timeout() const {
        return sa_stopwatch.elapsed_ms() >= time_limit;
    }

    template<Arithmetic T>
    bool should_accept(T score, T new_score) {
        double slope = static_cast<double>(sa_stopwatch.elapsed_ms()) / time_limit;

        double current_temperature = temperature + (target_temperature - temperature) * slope;

        T diff = new_score - score;
        if (diff > 0) {
            return true;
        }

        return exp(static_cast<double>(diff) / current_temperature) > randf();
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

    T &next() {
        std::vector<candidate> next_beam;

        for (int i = 0; i < static_cast<int>(beam.size()); ++i) {
            const T &state = beam[i];
            for (auto next_op: state.expand()) {
                next_beam.emplace_back(state.calculate_next_score(next_op), i, next_op);
            }
        }

        if (next_beam.empty()) {
            return beam.front();
        }

        int effective_beam_width = std::min(beam_width, static_cast<int>(next_beam.size()));

        std::nth_element(next_beam.begin(),
                         next_beam.begin() + effective_beam_width,
                         next_beam.end(),
                         [](const auto &a, const auto &b) {
                             return a.score > b.score;
                         });
        std::vector<T> composited_beam;
        for (int i = 0; i < effective_beam_width; ++i) {
            T new_state = beam[next_beam[i].state_index];
            new_state.next(next_beam[i].operation);
            composited_beam.push_back(std::move(new_state));
        }
        beam = std::move(composited_beam);

        return beam.front();
    }
};

#endif //ATCODERC_META_HEURISTICS_HPP
