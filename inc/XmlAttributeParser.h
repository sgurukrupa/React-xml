#pragma once
#include <string>
#include <map>
#include <memory>
#include "CppUtils.h"

namespace Xml
{
    using cpputils::tstring;

    typedef std::map<tstring, tstring> AttributeMap;

    class LIB_EXPORTS AttributeParser
    {
        tstring attr;

    public:
        AttributeParser(const tstring& attrToken);
        bool NextToken();
        tstring Key, Value;

        static std::unique_ptr<AttributeMap> Mapify(const tstring &attrToken)
        {
            AttributeParser ap(attrToken);
            auto *pm = new std::map<tstring, tstring>();
            while (ap.NextToken())
            {
                (*pm)[ap.Key] = ap.Value;
            }
            return std::unique_ptr<AttributeMap>(pm);
        }
    };
}

