#pragma once 

#include "fnv1a.hpp"

namespace di {
    /**
     * @brief Constant type index.
     */
    enum class const_index : fnv1a_result_t {};
    
    struct ctti {
        template<typename T>
        static constexpr const_index id();
    };

    template<typename T>
    constexpr const_index constid = ctti::id<T>();
}

#include "ctti.inl"
