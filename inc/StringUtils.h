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

    inline tstring Substring(const tstring &str, tstring::size_type start, tstring::size_type end)
    {
        const tstring::difference_type n = end - start;
        return n > 0 ? str.substr(start, n) : _T("");
    }

    class Tokenizer
    {
        tstring source;
        const tstring delim;
        tstring token, removedDelim;

    public:
        const tstring &RemovedDelimiter;
        Tokenizer(const tstring &source, const tstring &delimiters)
            : source(source), delim(delimiters), RemovedDelimiter(removedDelim)
        { }
        bool NextToken();
        const tstring &GetToken();
        const tstring &WhatRemains()
        {
            return source;
        }
        void PutBack(const tstring &token)
        {
            source.insert(0, token);
        }
    };

    class GroupTokenizer
    {
        Tokenizer toknzr;
        std::pair<TCHAR, TCHAR> grprPair;
        //std::map<TCHAR, TCHAR> toknGrprs;
        tstring groupedToken, tokenBuffer;

    public:
        GroupTokenizer(const tstring &source, const tstring &delimiters, std::pair<TCHAR, TCHAR> grprPair)
            : toknzr(source, delimiters), grprPair(grprPair)
        { }

        bool NextToken()
        {
            if (!tokenBuffer.empty())
            {
                tokenBuffer.clear();
                return true;
            }
            else if (toknzr.NextToken())
            {
                auto &token = toknzr.GetToken();
                auto i = token.find(grprPair.first);
                if (i != tstring::npos)
                {
                    tokenBuffer = token.substr(0, i); // save for later use
                    auto j = token.find(grprPair.second, i + 1); // is the pairing character inside the token itself?
                    if (j != tstring::npos)
                    {
                        groupedToken = Substring(token, i + 1, j);
                    }
                    else
                    {
                        groupedToken = token.substr(i + 1) + toknzr.RemovedDelimiter;
                        const auto &s = toknzr.WhatRemains();
                        j = s.find(grprPair.second);
                        groupedToken.append(s.substr(0, j));
                        while (toknzr.NextToken() && toknzr.GetToken().find(grprPair.second) == tstring::npos)
                            ; // skip tokens till we reach the grouper character
                    }
                }
                else
                {
                    groupedToken = token;
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        const tstring &GetToken()
        {
            return !tokenBuffer.empty() ? tokenBuffer : groupedToken;
        }
    };
}

