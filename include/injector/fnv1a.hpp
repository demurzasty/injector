#pragma once 

#include <cstdint>
#include <cstddef>
#include <string>

// todo: 64 bit version of fnv1a

namespace di {
    using fnv1a_result_t = std::uint32_t;

    constexpr fnv1a_result_t fnv1a(const char* str, std::size_t count);

    template<std::size_t N>
    constexpr fnv1a_result_t fnv1a(const char(&str)[N]);

    inline fnv1a_result_t fnv1a(const std::string& str);

    constexpr fnv1a_result_t operator"" _fnv1a(const char* str, std::size_t count);
}

#include "fnv1a.inl"

