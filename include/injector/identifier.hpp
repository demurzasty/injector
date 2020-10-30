#pragma once 

#include "ctti.hpp"

#include <typeinfo>
#include <typeindex>

namespace di {
    struct dependency_runtime_identifier {
        using index_type = std::type_index;

        template<typename T>
        static inline index_type id();
    };

    struct dependency_compiletime_identifier {
        using index_type = const_index;

        template<typename T>
        static constexpr index_type id();
    };

    using dependency_default_identifier = dependency_runtime_identifier;
}

#include "identifier.inl"
