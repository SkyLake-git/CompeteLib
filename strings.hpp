#ifndef ATCODERC_STRINGS_HPP
#define ATCODERC_STRINGS_HPP
#include <regex>
#include <string>

inline std::string base_convert(const std::string &str, const int n, const int m) {
    unsigned long long sum = 0;
    for (char c: str) {
        sum = sum * n + (c - '0');
    }
    std::string res;
    do {
        unsigned long long num = sum % m;
        res = static_cast<char>(num + '0') + res;
        sum /= m;
    } while (sum);

    return res;
}

inline std::string simple_replace(const std::string &victim, const std::string &from, const std::string &to) {
    return std::regex_replace(victim, std::regex(from), to);
}
#endif //ATCODERC_STRINGS_HPP
