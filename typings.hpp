#ifndef ATCODERC_TYPINGS_HPP
#define ATCODERC_TYPINGS_HPP
#include <type_traits>


template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template<typename T>
concept GraphLike = requires(T &a)
{
    { a.get_next() } -> std::convertible_to<std::vector<int>>;
    a.get_next(std::declval<int>());
};

template<typename T>
concept GridLike = requires(T &a)
{
    { a[std::declval<size_t>()] };
    sizeof(a);
};

enum DistanceAlgo {
    Euclidean,
    Manhattan
};

template<typename T>
concept DataContainer = requires
                        {
                            typename T::value_type;
                        } &&
                        (std::is_same_v<T, std::queue<typename T::value_type>> ||
                         std::is_same_v<T, std::stack<typename T::value_type>> ||
                         std::is_same_v<T, std::priority_queue<typename T::value_type>>);

template<DataContainer T>
void push_to_data_container(T &container, typename T::value_type v) {
    container.push(v);
}

template<DataContainer T>
T::value_type peek_from_data_container(T &container) {
    if constexpr (std::is_same_v<T, std::queue<typename T::value_type>>) {
        return container.front();
    } else if constexpr (std::is_same_v<T, std::stack<typename T::value_type>>) {
        return container.top();
    } else if constexpr (std::is_same_v<T, std::priority_queue<typename T::value_type>>) {
        return container.top();
    }

    throw std::runtime_error("unexpected type of data container");
}

template<DataContainer T>
void pop_from_data_container(T &container) {
    container.pop();
}

#endif //ATCODERC_TYPINGS_HPP
