#pragma once
#include <string>
#include <vector>
#include <queue>
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

    template <class T> int Vsnprintf(T *buf, size_t n, const T *fmt, va_list);
    template <> inline int Vsnprintf<char>(char *buf, size_t n, const char *fmt, va_list argptr)
    {
        return _vsnprintf(buf, n, fmt, argptr);
    }
    template <> inline int Vsnprintf<wchar_t>(wchar_t *buf, size_t n, const wchar_t *fmt, va_list argptr)
    {
        return _vsnwprintf(buf, n, fmt, argptr);
    }

    template<class TC> inline std::basic_string<TC> Format(const TC *const fmt, ...)
    {
        va_list a;
        va_start(a, fmt);
        TC buf[20];
        auto result = Vsnprintf<TC>(buf, sizeof buf / sizeof *buf, fmt, a);
        va_end(a);
        return std::basic_string<TC>(buf, result != -1 ? result : sizeof buf / sizeof *buf);
    }

    inline tstring Substring(const tstring &str, tstring::size_type start, tstring::size_type end)
    {
        const tstring::difference_type n = end - start;
        return n > 0 ? str.substr(start, n) : _T("");
    }

    inline tstring::size_type Replace(const tstring &str, tstring &in, const tstring &with, const tstring::size_type off = 0)
    {
        tstring::size_type ix;
        bool firstPass = true;
        const auto withSize = with.size();
        while ((ix = in.find(str, firstPass ? off : ix + withSize)) != tstring::npos)
        {
            in.replace(ix, str.size(), with);
        }
        return ix;
    }

    class Tokenizer
    {
        tstring source;
        tstring delimiters;
        tstring token, removedDelim;

    public:
        const tstring &RemovedDelimiter;
        Tokenizer(const tstring &source, const tstring &delimiters)
            : source(source), delimiters(delimiters), RemovedDelimiter(removedDelim)
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
        void AddDelimiter(TCHAR c)
        {
            delimiters.push_back(c);
        }
    };

    class GroupTokenizer
    {
        Tokenizer toknzr;
        std::pair<TCHAR, TCHAR> grprPair;
        //std::map<TCHAR, TCHAR> toknGrprs;
        tstring groupedToken;
        enum GRPSTATE { NOGRP, GRPSTART, INGRP, GRPEND } state;
        bool isDelimiterGrp()
        {
            return toknzr.RemovedDelimiter == tstring(1, grprPair.first);
        }

    public:
        GroupTokenizer(const tstring &source, const tstring &delimiters, std::pair<TCHAR, TCHAR> grprPair)
            : toknzr(source, delimiters), grprPair(grprPair), state(NOGRP)
        {
            toknzr.AddDelimiter(grprPair.first);
        }

        bool NextToken()
        {
            while (toknzr.NextToken())
            {
                auto &token = toknzr.GetToken();
                switch (state)
                {
                case NOGRP:
                case GRPEND:
                    groupedToken = token;
                    if (isDelimiterGrp()) state = GRPSTART;
                    return true;
                case GRPSTART:
                    groupedToken = token;
                    state = isDelimiterGrp() ? GRPEND : INGRP;
                    if (state == INGRP) groupedToken.append(toknzr.RemovedDelimiter);
                    if (state == GRPEND) return true;
                    break;
                case INGRP:
                    groupedToken.append(token);
                    state = isDelimiterGrp() ? GRPEND : INGRP;
                    if (state == INGRP) groupedToken.append(toknzr.RemovedDelimiter);
                    if (state == GRPEND) return true;
                }
            }
            return false;
        }

        const tstring &GetToken()
        {
            return groupedToken;
        }
    };
}

