#pragma once 

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace di {
    class dependency_container;

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
        constexpr std::size_t max_constructor_arguments = 10;

        struct dependency_bean {
            dependency_lifetime lifetime = dependency_lifetime::singleton;
            std::function<std::shared_ptr<void>()> factory;
            std::shared_ptr<void> instance;
        };

        struct dependency_injector {
            dependency_container& container;

            template<typename T>
            operator std::shared_ptr<T>();
        };

        template<int>
        using forward_injector = dependency_injector;

        template<typename T, int... ints>
        constexpr bool is_constructible(std::integer_sequence<int, ints...> seq) {
            return std::is_constructible_v<T, forward_injector<ints>...>;
        }

        template<typename T, int size>
        constexpr int constructor_argument_count() {
            if constexpr (size > 0) {
                return is_constructible<T>(std::make_integer_sequence<int, size>{}) ? size : constructor_argument_count<T, size - 1>();
            } else {
                return std::is_constructible_v<T> ? 0 : -1;
            }
        }

        template<typename T>
        constexpr int constructor_argument_count() {
            return constructor_argument_count<T, max_constructor_arguments>();
        }

        template<std::size_t>
        dependency_injector make_injector(di::dependency_container& container) {
            return { container };
        }

        template<typename T, int... ints>
        std::shared_ptr<T> resolve(dependency_container& container, std::integer_sequence<int, ints...> seq) {
            return std::make_shared<T>(make_injector<ints>(container)...);
        }

        template<typename T>
        std::shared_ptr<T> resolve(di::dependency_container& container) {
            constexpr auto args = constructor_argument_count<T>();
            static_assert(args > -1, "No suitable constructor found");
            return resolve<T>(container, std::make_integer_sequence<int, args>{});
        }
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
            return detail::resolve<T>(*this);
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

    template<typename T>
    detail::dependency_injector::operator std::shared_ptr<T>() {
        return container.get<T>();
    }
}
