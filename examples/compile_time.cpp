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
    di::basic_dependency_container<di::dependency_compiletime_identifier> container;

    container.install<logger, stdout_logger>(di::dependency_lifetime::singleton);

    auto logger = container.get<::logger>();

    logger->log("Hello World!");
}
