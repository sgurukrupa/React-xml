#pragma once
#ifdef _CPPUTILS_
#define LIB_EXPORTS __declspec(dllexport)
#elif _USE_CPPUTILS_
#define LIB_EXPORTS __declspec(dllimport)
#else
#define LIB_EXPORTS
#endif

#define _UNICODE 1

#include <tchar.h>
#include <string>
#include <fstream>
#include <sstream>

namespace cpputils
{
    typedef std::basic_string<TCHAR> tstring;
    typedef std::basic_ifstream<TCHAR> tifstream;
    typedef std::basic_ofstream<TCHAR> tofstream;
    typedef std::basic_istringstream<TCHAR> tistringstream;
    typedef std::basic_ostringstream<TCHAR> tostringstream;
}

