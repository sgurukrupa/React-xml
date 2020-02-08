#pragma once

#include <string>
#include <fstream>
#include <sstream>

#define LIB_EXPORTS
#define TCHAR char
#define _T(s) s

namespace reactxml
{
  namespace cpputils
  {
    using tstring = std::basic_string<char>;
    using tifstream = std::basic_ifstream<char>;
    using tofstream = std::basic_ofstream<char>;
    using tistringstream = std::basic_istringstream<char>;
    using tostringstream = std::basic_ostringstream<char>;
  }
}

