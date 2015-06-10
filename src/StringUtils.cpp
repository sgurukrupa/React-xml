#include "StringUtils.h"

namespace StringUtils
{
    cpputils::tstring Format(const TCHAR * const fmt, ...)
    {
        va_list a;
        va_start(a, fmt);
        TCHAR buf[100];
        bool done = false;
        done = _vsntprintf(buf, sizeof buf, fmt, a) < sizeof buf;
        va_end(a);
        return done ? buf : _T("");
    }

    bool Tokenizer::NextToken()
    {
        if (source.empty()) return false;
        auto ix = source.find_first_of(delimiters);
        if (ix == tstring::npos) ix = source.length();
        removedDelim = source.substr(ix, 1);
        token = source.substr(0, ix);
        source.erase(0, ix + 1); // erase the delimiter too
        return true;
    }

    const tstring &Tokenizer::GetToken()
    {
        return token;
    }
}

