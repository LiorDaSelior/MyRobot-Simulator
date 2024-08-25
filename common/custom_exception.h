#include <exception>
#include <string>

class AlgorithmSegFault : public std::exception {
private:
    std::string message;

public:
    explicit AlgorithmSegFault(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};