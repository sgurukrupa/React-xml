#pragma once
#include <string>
#include <map>
#include <memory>
#include "CppUtils.h"

namespace Xml
{
    using cpputils::tstring;
    using std::map;

    class LIB_EXPORTS AttributeParser
    {
        tstring attr;

    public:
        AttributeParser(const tstring& attrToken);
        bool NextToken();
        tstring Key, Value;

        static std::unique_ptr<const map<tstring, tstring>> Mapify(const tstring &attrToken)
        {
            AttributeParser ap(attrToken);
            auto *pm = new std::map<tstring, tstring>();
            while (ap.NextToken())
            {
                (*pm)[ap.Key] = ap.Value;
            }
            return std::unique_ptr<const map<tstring, tstring>>(pm);
        }
    };
}

