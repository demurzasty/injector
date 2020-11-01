#pragma once 

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace di {
    /**
     * @brief Determine lifetime of dependency. 
     * 
     *        Singleton lifetime has a lazy evaluation of instance
     *        and may exists only one instance of this dependency.
     * 
     *        Transient lifetime creates instance each resolving. 
     * 
     * @todo Add @a scoped lifetime to creates instance each request. 
     */
    enum class dependency_lifetime {
        transient,
        singleton,
    };

    namespace detail {
        struct dependency_bean {
            dependency_lifetime lifetime = dependency_lifetime::singleton;
            std::function<std::shared_ptr<void>()> factory;
            std::shared_ptr<void> instance;
        };
    }

    /**
     * @brief Main container for dependencies. Threadsafe.
     */
    class dependency_container {
    public:
        /**
         * @brief Install dependency statically with provided instance as singleton lifetime.
         * 
         * @param instance Instance of T.
         */
        template<typename T>
        void install(std::shared_ptr<T> instance) {
            _beans.emplace(typeid(T), detail::dependency_bean{
                dependency_lifetime::singleton,
                nullptr,
                instance
            });
        }

        /**
         * @brief Install dependency with provided lifetime.
         * 
         * @param lifetime Lifetime of dependency.
         */
        template<typename T>
        void install(dependency_lifetime lifetime) {
            static_assert(!std::is_abstract_v<T>, "Cannot install abstract class - provide implementation");

            _beans.emplace(typeid(T), detail::dependency_bean{
                lifetime,
                [this] { return resolve<T>(); },
                nullptr
            });
        }

        /**
         * @brief Install dependency with provided lifetime.
         * 
         * @param lifetime Lifetime of dependency.
         */
        template<typename Interface, typename Implementation>
        void install(dependency_lifetime lifetime) {
            static_assert(!std::is_abstract_v<Implementation>, "Cannot install abstract class");

            _beans.emplace(typeid(Interface), detail::dependency_bean{
                lifetime,
                [this] { return resolve<Implementation>(); },
                nullptr
            });
        }

        /**
         * @brief Install dependency with provided lifetime and custom resolver.
         *        It's useful when you want to hide implementation of derived class
         *        and it is not visible publicly, i.e. platform specific implementations.
         *
         * @param lifetime Lifetime of dependency.
         * @param resolver Resolver.
         */
        template<typename T>
        void install(dependency_lifetime lifetime, std::shared_ptr<T>(*resolver)(dependency_container&)) {
            _beans.emplace(typeid(T), detail::dependency_bean{
                lifetime,
                [this, resolver]() { return resolver(*this); },
                nullptr
            });
        }

        /**
         * @brief Get (and create if needed) dependency. 
         */
        template<typename T>
        std::shared_ptr<T> get() {
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

        /**
         * @brief Resolve instance. 
         */
        template<typename T>
        std::shared_ptr<T> resolve() {
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

        /**
         * @brief Tell whether dependency is installed.
         */
        template<typename T>
        bool installed() const {
            return _beans.find(typeid(T)) != _beans.end();
        }

        /**
         * @brief Returns lifetime of dependency.
         */
        template<typename T>
        dependency_lifetime lifetime() const {
            return _beans.at(typeid(T)).lifetime;
        }

    private:
        std::unordered_map<std::type_index, detail::dependency_bean> _beans;
    };

    /**
     * @brief Helper class for dependency injection to constructor.
     */
    struct dependency_injector {
        dependency_container& container;

        /**
         * @brief 
         */
        template<typename T>
        operator std::shared_ptr<T>() {
            return container.get<T>();
        }
    };
}
