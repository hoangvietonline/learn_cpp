#ifndef SDP_ERRORS_H
#define SDP_ERRORS_H

#include <stdexcept>
#include <string>

namespace sdp
{
    class Error : public std::runtime_error
    {
    public:
        explicit Error(const std::string &message)
            : std::runtime_error(message) {}
    };

    class EmptyContainerError : public Error
    {
    public:
        explicit EmptyContainerError(const std::string &message)
            : Error(message) {}
    };

    class ParseError : public Error
    {
    public:
        explicit ParseError(const std::string &message)
            : Error(message) {}
    };

    class FileError : public Error
    {
    public:
        explicit FileError(const std::string &message)
            : Error(message) {}
    };

}

#endif