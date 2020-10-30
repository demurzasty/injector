#pragma once 

#include "identifier.hpp"

#include <typeinfo>
#include <typeindex>

namespace di {
    template<typename T>
    static inline dependency_runtime_identifier::index_type dependency_runtime_identifier::id() {
        return typeid(T);
    }

    template<typename T>
    static constexpr dependency_compiletime_identifier::index_type dependency_compiletime_identifier::id() {
        return constid<T>;
    }
}
