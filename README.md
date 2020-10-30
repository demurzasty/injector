# injector

[![Build Status](https://github.com/demurzasty/injector/workflows/CMake/badge.svg)](https://github.com/skypjack/entt/actions)  
*injector* is a header-only, tiny and easy to use library for dependency injection written in *C++17*.

## Code Examples

### Basic Usage
```cpp
#include <injector/injector.hpp>

#include <string>
#include <iostream>
#include <fstream>

struct logger {
    virtual ~logger() = default;
    virtual void log(const std::string& message) = 0;
};

struct stdout_logger : public logger {
    void log(const std::string& message) override {
        std::cout << message << std::endl;
    }
};

struct file_logger : public logger {
    void log(const std::string& message) override {
        std::ofstream("log.txt", std::ios::app) << message << std::endl;
    }
};

int main(int argc, char* argv[]) {
    di::dependency_container container;

    container.install<logger, stdout_logger>(di::dependency_lifetime::singleton);

    auto& logger = container.get<::logger>();

    logger->log("Hello World!");
}
```

### Dependency Injection
```cpp
#include <injector/injector.hpp>

#include <string>
#include <iostream>
#include <fstream>

struct logger {
    void log(const std::string& message) {
        std::cout << message << std::endl;
    }
};

struct important_service  {
    important_service(std::shared_ptr<logger> logger)
        : _logger(logger) {
    }

    void do_something_important() {
        _logger->log("do_something_important()");
    }

    std::shared_ptr<logger> _logger;
};


int main(int argc, char* argv[]) {
    di::dependency_container container;

    container.install<logger>(di::dependency_lifetime::singleton);
    container.install<important_service>(di::dependency_lifetime::singleton);

    auto service = container.get<important_service>();

    service->do_something_important();
}
```
### Resolver
```cpp
#include <injector/injector.hpp>

#include <string>
#include <iostream>
#include <fstream>

struct logger {
    static std::shared_ptr<logger> resolver(di::dependency_container& container);
    virtual ~logger() = default;
    virtual void log(const std::string& message) = 0;
};

struct stdout_logger : public logger {
    void log(const std::string& message) override {
        std::cout << message << std::endl;
    }
};

std::shared_ptr<logger> logger::resolver(di::dependency_container& container) {
    return container.resolve<stdout_logger>();
}

int main(int argc, char* argv[]) {
    di::dependency_container container;

    container.install<logger>(di::dependency_lifetime::singleton, &logger::resolver);

    auto logger = container.get<::logger>();

    logger->log("Hello World!");
}
```
