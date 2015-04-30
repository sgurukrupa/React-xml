#pragma once
#include <fstream>
#include <string>
#include <stack>
#include <map>
#include <memory>
#include "CppUtils.h"
#include "XmlAttributeParser.h"

namespace Xml
{
    enum TOKEN_TYPE { BODY, START_TAG, ATTRIBUTE, END_TAG, COMMENT, INSTRUCTION }; // ATTRIBUTE is more specific case of START_TAG

    using cpputils::tstring;
    using std::map;

    struct LIB_EXPORTS TokenResult
    {
        tstring Value;
        TOKEN_TYPE Type;
        bool EndOfStartTag; // only applicable when Type == START_TAG or ATTRIBUTE
        bool NoBody; // only applicable when EndOfStartTag is true
        TokenResult() : Type(BODY), EndOfStartTag(false), NoBody(false) { }
        std::unique_ptr<const map<tstring, tstring>> Attributify() const
        {
            if (Type != ATTRIBUTE) throw Value; // error
            return AttributeParser::Mapify(Value);
        }
        tstring GetAttributeValue(tstring attr) const
        {
            auto a = Attributify();
            auto it = a->find(attr);
            if (it == a->end()) throw attr; // error: no such attribute
            return it->second;
        }
    };

    struct IParser
    {
        virtual bool NextToken() = 0;
        virtual const TokenResult &GetToken() = 0;
        virtual const tstring &GetElement() = 0;
        virtual bool SkipToStartTag(bool notBeyondEndTag = true) = 0;
        virtual bool SkipToEndTag() = 0;
    };

    class LIB_EXPORTS Parser : public IParser
    {
        std::ifstream xmlfile;
        TokenResult tr;
        std::stack<tstring> startedTags;
        size_t lineNo;

    public:
        Parser(const tstring &filename);
        bool NextToken();
        const TokenResult &GetToken();
        const tstring &GetElement();
        bool SkipToStartTag(bool notBeyondEndTag = true);
        bool SkipToEndTag()
        {
            while (NextToken())
            {
                if (GetToken().Type == END_TAG) return true;
            }
            return false;
        }
        ~Parser();
    };
}
