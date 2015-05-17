#pragma once
#include <string>
#include <vector>
#include <cstdarg>
#include "CppUtils.h"

namespace StringUtils
{
    using cpputils::tstring;
    static const TCHAR *WHITESPACE = _T(" \t\n\r\f");

    inline tstring Ltrim(const tstring &str)
    {
        auto i = str.find_first_not_of(WHITESPACE);
        if (i == tstring::npos) return _T("");
        return str.substr(i);
    }

    inline tstring Rtrim(const tstring &str)
    {
        auto i = str.find_last_not_of(WHITESPACE);
        if (i == tstring::npos) return _T("");
        return str.substr(0, i + 1);
    }

    inline tstring Trim(const tstring &str)
    {
        return Ltrim(Rtrim(str));
    }

    template <class T>
    inline tstring Stringify(T strings)
    {
        tstring s;
        for (T::const_iterator it = strings.begin(), end = strings.end(); it != end; ++it)
        {
            s.append(*it).append(it + 1 != end ? _T(", ") : _T(""));
        }
        return s;
    }

    tstring Format(const TCHAR * const fmt, ...);

    class Tokenizer
    {
        tstring source;
        const tstring delim;
        tstring token, removedDelim;

    public:
        const tstring &RemovedDelimiter;
        Tokenizer(const tstring &source, const tstring &delim)
            : source(source), delim(delim), RemovedDelimiter(removedDelim)
        { }
        bool NextToken();
        const tstring &GetToken();
        const tstring &WhatRemains()
        {
            return source;
        }
    };

    class GroupTokenizer
    {
        Tokenizer toknzr;
        const TCHAR toknGrpr;
        tstring groupedToken;

    public:
        GroupTokenizer(const tstring &source, const tstring &delim, TCHAR tokenGrouper = _T('"'))
            : toknzr(source, delim), toknGrpr(tokenGrouper)
        { }

        bool NextToken()
        {
            if (toknzr.NextToken())
            {
                auto &token = toknzr.GetToken();
                auto i = token.find(toknGrpr);
                if (i != tstring::npos)
                {
                    groupedToken = token.substr(i + 1) + toknzr.RemovedDelimiter;
                    const auto &s = toknzr.WhatRemains();
                    auto i = s.find(toknGrpr);
                    groupedToken.append(s.substr(0, i));
                    while (toknzr.NextToken() && toknzr.GetToken().find(toknGrpr) == tstring::npos)
                        ;
                }
                else
                {
                    groupedToken = token;
                }
                return true;
            }
            return false;
        }

        const tstring &GetToken()
        {
            return groupedToken;
        }
    };
}

