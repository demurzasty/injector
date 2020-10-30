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
        struct dependency_bean {
            dependency_lifetime lifetime = dependency_lifetime::singleton;
            std::function<std::shared_ptr<void>()> factory;
            std::shared_ptr<void> instance;
        };
    }

    /**
     * @brief External dependency resolver. 
     */
    using dependency_resolver = std::function<std::shared_ptr<void>(dependency_container&)>;

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
        void install(std::shared_ptr<T> instance);

        /**
         * @brief Install dependency with provided lifetime.
         * 
         * @param lifetime Lifetime of dependency.
         */
        template<typename T>
        void install(dependency_lifetime lifetime);

        /**
         * @brief Install dependency with provided lifetime.
         * 
         * @param lifetime Lifetime of dependency.
         */
        template<typename Interface, typename Implementation>
        void install(dependency_lifetime lifetime);

        /**
         * @brief Install dependency with provided lifetime and factory.
         * 
         * @param lifetime Lifetime of dependency.
         * @param factory Factory function returning T.
         */
        template<typename T, typename Factory>
        void install(dependency_lifetime lifetime, Factory factory);

        /**
         * @brief Install dependency with provided lifetime and factory.
         * 
         * @param lifetime Lifetime of dependency.
         * @param factory Factory function returning Implementation.
         */
        template<typename Interface, typename Implementation, typename Factory>
        void install(dependency_lifetime lifetime, Factory factory);

        /**
         * @brief Install dependency with provided lifetime and custom resolver.
         *        It's useful when you want to hide implementation of derived class
         *        and it is not visible publicly, i.e. platform specific implementations.
         *
         * @param lifetime Lifetime of dependency.
         * @param resolver Resolver.
         */
        template<typename T>
        void install(dependency_lifetime lifetime, const dependency_resolver& resolver);

        /**
         * @brief Get (and create if needed) dependency. 
         */
        template<typename T>
        std::shared_ptr<T> get();

        /**
         * @brief Resolve instance. 
         */
        template<typename T>
        std::shared_ptr<T> resolve();

        /**
         * @brief Tell whether dependency is installed.
         */
        template<typename T>
        bool installed() const;

        /**
         * @brief Returns lifetime of dependency.
         */
        template<typename T>
        dependency_lifetime lifetime() const;

    private:
        std::unordered_map<std::type_index, detail::dependency_bean> _beans;
    };

    /**
     * @brief Helper class for dependency injection to constructor.
     */
    struct dependency_injector {
        dependency_container& container;

        template<typename T>
        operator std::shared_ptr<T>();
    };
}

#include "injector.inl"
