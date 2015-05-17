#pragma once
#ifdef _CPPUTILS_
#define LIB_EXPORTS __declspec(dllexport)
#elif _USE_CPPUTILS_
#define LIB_EXPORTS __declspec(dllimport)
#else
#define LIB_EXPORTS
#endif

#include <tchar.h>
#include <string>

namespace cpputils
{
    typedef std::basic_string<TCHAR> tstring;
}

