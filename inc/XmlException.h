#pragma once
#include <string>

namespace Xml
{
    class Exception : public std::exception
    {
        std::string reason;
    public:
        Exception(const std::string &reason) : reason(reason) { }
        const char *what() const
        {
            return reason.c_str();
        }
    };
}

