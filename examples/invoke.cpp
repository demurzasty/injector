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

void log_hello_world(std::shared_ptr<logger> logger) {
    std::cout << "Hello World!" << std::endl;
}

int main(int argc, char* argv[]) {
    di::dependency_container container;

    container.install<logger, stdout_logger>(di::dependency_lifetime::singleton);

    container.invoke(&log_hello_world);
}
