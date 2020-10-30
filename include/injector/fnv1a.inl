#pragma once 

#include "fnv1a.hpp"

namespace di {
    constexpr fnv1a_result_t fnv1a(const char* str, std::size_t count) {
        return ((count ? fnv1a(str, count - 1) : 2166136261u) ^ str[count]) * 16777619u;
    }

    template<std::size_t N>
    constexpr fnv1a_result_t fnv1a(const char(&str)[N]) {
        return fnv1a(str, N - 1);
    }

    inline fnv1a_result_t fnv1a(const std::string& str) {
        return fnv1a(str.c_str(), str.size());
    }

    constexpr fnv1a_result_t operator"" _fnv1a(const char* str, std::size_t count) {
        return fnv1a(str, count);
    }
}
