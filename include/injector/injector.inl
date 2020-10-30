#pragma once 

#include "injector.hpp"

#include <cassert>
#include <type_traits>

namespace di {
    template<typename T>
    void dependency_container::install(std::shared_ptr<T> instance) {
        _beans.emplace(typeid(T), detail::dependency_bean{
            dependency_lifetime::singleton,
            nullptr,
            instance
        });
    }

    template<typename T>
    void dependency_container::install(dependency_lifetime lifetime) {
        static_assert(!std::is_abstract_v<T>, "Cannot install abstract class - provide implementation");

        _beans.emplace(typeid(T), detail::dependency_bean{
            lifetime,
            [this] { return resolve<T>(); },
            nullptr
        });
    }

    template<typename Interface, typename Implementation>
    void dependency_container::install(dependency_lifetime lifetime) {
        static_assert(!std::is_abstract_v<Implementation>, "Cannot install abstract class");

        _beans.emplace(typeid(Interface), detail::dependency_bean{
            lifetime,
            [this] { return resolve<Implementation>(); },
            nullptr
        });
    }

    template<typename T>
    void dependency_container::install(dependency_lifetime lifetime, std::shared_ptr<T>(*resolver)(dependency_container&)) {
        _beans.emplace(typeid(T), detail::dependency_bean{
            lifetime,
            [this, resolver]() { return resolver(*this); },
            nullptr
        });
    }

    template<typename T>
    std::shared_ptr<T> dependency_container::get() {
        auto& bean = _beans.at(typeid(T));
        if (bean.lifetime == dependency_lifetime::singleton) {
            if (!bean.instance) {
                bean.instance = bean.factory();
            }
            return std::static_pointer_cast<T>(bean.instance);
        } else {
            return std::static_pointer_cast<T>(bean.factory());
        }
    }

    template<typename T>
    bool dependency_container::installed() const {
        return _beans.find(typeid(T)) != _beans.end();
    }

    template<typename T>
    dependency_lifetime dependency_container::lifetime() const {
        return _beans.at(typeid(T)).lifetime;
    }

    template<typename T>
    std::shared_ptr<T> dependency_container::resolve() {
        dependency_injector injector{ *this };
        if constexpr (std::is_constructible_v<T>) {
            return std::make_shared<T>();
        } else if constexpr (std::is_constructible_v<T, dependency_injector>) {
            return std::make_shared<T>(injector);
        } else if constexpr (std::is_constructible_v<T, dependency_injector, dependency_injector>) {
            return std::make_shared<T>(injector, injector);
        } else if constexpr (std::is_constructible_v<T, dependency_injector, dependency_injector, dependency_injector>) {
            return std::make_shared<T>(injector, injector, injector);
        } else if constexpr (std::is_constructible_v<T, dependency_injector, dependency_injector, dependency_injector, dependency_injector>) {
            return std::make_shared<T>(injector, injector, injector, injector);
        } else if constexpr (std::is_constructible_v<T, dependency_injector, dependency_injector, dependency_injector, dependency_injector, dependency_injector>) {
            return std::make_shared<T>(injector, injector, injector, injector, injector);
        } else {
            static_assert(false, "No suitable constructor found");
        }
    }

    template<typename T>
    dependency_injector::operator std::shared_ptr<T>() {
        return container.get<T>();
    }    
}
