#ifndef VERIFICATION_EXCEPTIONS_H_
#define VERIFICATION_EXCEPTIONS_H_

#include <exception>
#include <string>

class VerificationException : public std::exception {
public:
    explicit VerificationException(std::string const &message): message{message} {}
    const char *what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

class BoundednessException : public VerificationException {
public:
    explicit BoundednessException(std::string const &message): VerificationException(message) {};
};

class LivenessException : public VerificationException {
public:
    explicit LivenessException(std::string const &message): VerificationException(message) {};
};

class ReachabilityException : public VerificationException {
public:
    explicit ReachabilityException(std::string const &message): VerificationException(message) {};
};

#endif  // VERIFICATION_EXCEPTIONS_H_

