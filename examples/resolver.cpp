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
