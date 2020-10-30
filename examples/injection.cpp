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
