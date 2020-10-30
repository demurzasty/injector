#pragma once 

#include "injector.hpp"

#include <cassert>
#include <type_traits>

namespace di {
    template<typename Identifier>
    template<typename T>
    void basic_dependency_container<Identifier>::install(std::shared_ptr<T> instance) {
        _beans.emplace(Identifier::id<T>(), detail::dependency_bean{
            dependency_lifetime::singleton,
            nullptr,
            instance
        });
    }

    template<typename Identifier>
    template<typename T>
    void basic_dependency_container<Identifier>::install(dependency_lifetime lifetime) {
        static_assert(!std::is_abstract_v<T>, "Cannot install abstract class - provide implementation");

        _beans.emplace(Identifier::id<T>(), detail::dependency_bean{
            lifetime,
            [this] { return resolve<T>(); },
            nullptr
        });
    }

    template<typename Identifier>
    template<typename Interface, typename Implementation>
    void basic_dependency_container<Identifier>::install(dependency_lifetime lifetime) {
        static_assert(!std::is_abstract_v<Implementation>, "Cannot install abstract class");

        _beans.emplace(Identifier::id<Interface>(), detail::dependency_bean{
            lifetime,
            [this] { return resolve<Implementation>(); },
            nullptr
        });
    }

    template<typename Identifier>
    template<typename T, typename Factory>
    void basic_dependency_container<Identifier>::install(dependency_lifetime lifetime, Factory factory) {
        _beans.emplace(Identifier::id<T>(), detail::dependency_bean{
            lifetime,
            [factory]() { return factory(); },
            nullptr
        });
    }

    template<typename Identifier>
    template<typename Interface, typename Implementation, typename Factory>
    void basic_dependency_container<Identifier>::install(dependency_lifetime lifetime, Factory factory) {
        _beans.emplace(Identifier::id<Interface>(), detail::dependency_bean{
            lifetime,
            [factory]() { return factory(); },
            nullptr
        });
    }

    template<typename Identifier>
    template<typename T>
    void basic_dependency_container<Identifier>::install(dependency_lifetime lifetime, const basic_dependency_resolver<Identifier>& resolver) {
        _beans.emplace(Identifier::id<T>(), detail::dependency_bean{
            lifetime,
            [this, resolver]() { return resolver(*this); },
            nullptr
        });
    }

    template<typename Identifier>
    template<typename T>
    std::shared_ptr<T> basic_dependency_container<Identifier>::get() {
        auto& bean = _beans.at(Identifier::id<T>());
        if (bean.lifetime == dependency_lifetime::singleton) {
            if (!bean.instance) {
                bean.instance = bean.factory();
            }
            return std::static_pointer_cast<T>(bean.instance);
        } else {
            return std::static_pointer_cast<T>(bean.factory());
        }
    }
    
    template<typename Identifier>
    template<typename T>
    bool basic_dependency_container<Identifier>::installed() const {
        return _beans.find(Identifier::id<T>()) != _beans.end();
    }

    template<typename Identifier>
    template<typename T>
    dependency_lifetime basic_dependency_container<Identifier>::lifetime() const {
        return _beans.at(Identifier::id<T>()).lifetime;
    }

    template<typename Identifier>
    template<typename T>
    std::shared_ptr<T> basic_dependency_container<Identifier>::resolve() {
        using injector_type = detail::basic_dependency_injector<Identifier>;

        injector_type injector{ *this };
        if constexpr (std::is_constructible_v<T>) {
            return std::make_shared<T>();
        } else if constexpr (std::is_constructible_v<T, injector_type>) {
            return std::make_shared<T>(injector);
        } else if constexpr (std::is_constructible_v<T, injector_type, injector_type>) {
            return std::make_shared<T>(injector, injector);
        } else if constexpr (std::is_constructible_v<T, injector_type, injector_type, injector_type>) {
            return std::make_shared<T>(injector, injector, injector);
        } else if constexpr (std::is_constructible_v<T, injector_type, injector_type, injector_type, injector_type>) {
            return std::make_shared<T>(injector, injector, injector, injector);
        } else if constexpr (std::is_constructible_v<T, injector_type, injector_type, injector_type, injector_type, injector_type>) {
            return std::make_shared<T>(injector, injector, injector, injector, injector);
        } else {
            static_assert(false, "No suitable constructor found");
        }
    }


    template<typename Identifier>
    template<typename T>
    detail::basic_dependency_injector<Identifier>::operator std::shared_ptr<T>() {
        return container.get<T>();
    }    
}
