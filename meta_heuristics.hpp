#ifndef ATCODERC_META_HEURISTICS_HPP
#define ATCODERC_META_HEURISTICS_HPP
#include "typings.hpp"
#include "utils.hpp"

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
        return exp(static_cast<double>(diff) / current_temperature) > randf();
    }
};

#endif //ATCODERC_META_HEURISTICS_HPP
