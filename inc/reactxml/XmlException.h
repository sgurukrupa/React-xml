#pragma once
#include <string>

namespace reactxml
{
    class Exception : public std::exception
    {
        std::string reason;
    public:
        Exception (std::string reason) : reason (std::move(reason)) { }

        const char *what() const noexcept override
        {
            return reason.c_str();
        }
    };
}

