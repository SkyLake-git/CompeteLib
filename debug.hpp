#pragma once

#include <cstdio>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace debug_detail {
    template<typename T>
    using plain_t = std::remove_cv_t<std::remove_reference_t<T>>;

    template<typename T>
    concept String = std::is_convertible_v<T, std::string_view>;

    template<typename T>
    concept Range = requires(const T &value)
    {
        std::begin(value);
        std::end(value);
    } && !String<T>;

    template<typename T>
    concept Tuple = requires
    {
        std::tuple_size<plain_t<T>>::value;
    };

    template<typename T>
    void write(std::ostream &out, const T &value, int indent = 0);

    inline std::string join(std::string_view separator, const std::vector<std::string> &values) {
        std::string result;
        for (const auto &value: values) {
            if (!result.empty()) result += separator;
            result += value;
        }
        return result;
    }

    template<typename... T>
    std::vector<std::string> write_all(int indent, const T &... value) {
        std::vector<std::string> result;
        result.reserve(sizeof...(value));
        (result.push_back([&] {
             std::ostringstream out;
             write(out, value, indent);
             return out.str();
         }()), ...);
        return result;
    }

    inline void write_string(std::ostream &out, std::string_view value) {
        out << '"';
        for (char c: value) {
            if (c == '"' || c == '\\') out << '\\';
            out << c;
        }
        out << '"';
    }

    template<typename T>
    void write_tuple(std::ostream &out, const T &value, int indent) {
        out << '(';
        std::apply([&](const auto &... item) {
            out << join(", ", write_all(indent, item...));
        }, value);
        out << ')';
    }

    template<typename T>
    void write(std::ostream &out, const T &value, int indent) {
        using U = plain_t<T>;
        if constexpr (String<T>) {
            write_string(out, std::string_view(value));
        } else if constexpr (std::is_same_v<U, std::nullptr_t>) {
            out << "nullptr";
        } else if constexpr (std::is_same_v<U, bool>) {
            out << (value ? "true" : "false");
        } else if constexpr (std::is_same_v<U, char>) {
            out << '\'' << value << '\'';
        } else if constexpr (std::is_arithmetic_v<U>) {
            out << value;
        } else if constexpr (requires { value.first; value.second; value.third; value.fourth; }) {
            out << '(' << join(", ", write_all(indent, value.first, value.second, value.third, value.fourth))
                << ')';
        } else if constexpr (requires { value.first; value.second; value.third; }) {
            out << '(' << join(", ", write_all(indent, value.first, value.second, value.third)) << ')';
        } else if constexpr (requires { value.first; value.second; }) {
            out << '(' << join(", ", write_all(indent, value.first, value.second)) << ')';
        } else if constexpr (requires { value.has_value(); *value; }) {
            if (value) {
                out << "Some(";
                write(out, *value, indent);
                out << ')';
            } else {
                out << "None";
            }
        } else if constexpr (requires
        {
            std::visit([](const auto &) {
            }, value);
        }) {
            std::visit([&](const auto &item) { write(out, item, indent); }, value);
        } else if constexpr (std::is_same_v<U, std::monostate>) {
            out << "monostate";
        } else if constexpr (Range<T>) {
            out << '[';
            bool first = true;
            for (const auto &item: value) {
                out << (first ? "\n" : ",\n") << std::string(indent + 2, ' ');
                write(out, item, indent + 2);
                first = false;
            }
            if (!first) out << '\n' << std::string(indent, ' ');
            out << ']';
        } else if constexpr (Tuple<T>) {
            write_tuple(out, value, indent);
        } else if constexpr (std::is_pointer_v<U>) {
            if (value == nullptr) out << "nullptr";
            else write(out, *value, indent);
        } else {
            out << value;
        }
    }

    template<typename... T>
    void print(const char *names, const char *file, int line, const T &... value) {
#ifndef ONLINE_JUDGE
        std::cerr << file << ':' << line << ": " << names << " = "
                  << join(", ", write_all(0, value...)) << '\n';
#else
        (void) names;
        (void) file;
        (void) line;
        (void) sizeof...(value);
#endif
    }
} // namespace debug_detail

#define dbg(...) \
    (::debug_detail::print(#__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__), (__VA_ARGS__))

template<typename... T>
void NOP_LOCAL_DEBUG(const T &...) {
}

#ifdef ONLINE_JUDGE
#define printf(...) NOP_LOCAL_DEBUG(__VA_ARGS__)
#else
#define printf(...) std::fprintf(stderr, __VA_ARGS__)
#endif

#ifdef ONLINE_JUDGE
constexpr bool IS_ONLINE_JUDGE = true;
#else
constexpr bool IS_ONLINE_JUDGE = false;
#endif
