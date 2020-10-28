#include <injector/injector.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <optional>

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

struct user {
    std::string email;
};

class user_repository {
public:
    void insert(const std::string& email) {
        _users.emplace(_counter++, user{ email });
    }

    std::optional<user> get_by_email(const std::string& email) {
        auto it = std::find_if(_users.begin(), _users.end(), [&](const std::pair<const std::size_t, user> pair) {
            return pair.second.email == email;
        });

        if (it != _users.end()) {
            return it->second;
        }
        return {};
    }

private:
    std::size_t _counter = 1;
    std::unordered_map<std::size_t, user> _users;
};

class registration_service {
public:
    registration_service(std::shared_ptr<logger> logger, std::shared_ptr<user_repository> repository)
        : _logger(logger)
        , _repository(repository) {
    }

    void register_user(const std::string& email) {
        if (_repository->get_by_email(email)) {
            _logger->log("User '" + email + "' is already registered");
        } else {
            _repository->insert(email);
            _logger->log("User '" + email + "' has been registered");
        }
    }

private:
    std::shared_ptr<logger> _logger;
    std::shared_ptr<user_repository> _repository;
};

int main(int argc, char* argv[]) {
    di::dependency_container container;

    container.install<logger, stdout_logger>(di::dependency_lifetime::singleton);
    container.install<user_repository>(di::dependency_lifetime::singleton);
    container.install<registration_service>(di::dependency_lifetime::singleton);

    auto registration_service = container.get<::registration_service>();

    registration_service->register_user("tom@email.com");
    registration_service->register_user("jerry@email.com");
    registration_service->register_user("tom@email.com");
}
