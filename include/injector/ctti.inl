#pragma once 

#include "ctti.hpp"

#include <type_traits>

namespace di {
    template<typename T>
    static constexpr const_index ctti::id() {
#ifdef _MSC_VER
        return static_cast<const_index>(fnv1a(__FUNCSIG__));
#else
        return static_cast<const_index>(fnv1a(__PRETTY_FUNCTION__));
#endif
    }
}
