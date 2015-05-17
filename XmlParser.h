#pragma once
#include <fstream>
#include <string>
#include <stack>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "CppUtils.h"
#include "XmlAttributeParser.h"
#include "StringUtils.h"

namespace Xml
{
    enum TOKEN_TYPE { BODY, START_TAG, ATTRIBUTE, END_TAG, COMMENT, INSTRUCTION }; // ATTRIBUTE is more specific case of START_TAG

    using cpputils::tstring;
    using std::map;
    using std::unique_ptr;
    using std::vector;

    struct LIB_EXPORTS TokenResult
    {
        tstring Value;
        TOKEN_TYPE Type;
        bool EndOfStartTag; // only applicable when Type == START_TAG or ATTRIBUTE
        bool NoBody; // only applicable when EndOfStartTag is true
        TokenResult() : Type(BODY), EndOfStartTag(false), NoBody(false) { }
        std::unique_ptr<AttributeMap> Attributify() const
        {
            if (Type != ATTRIBUTE) throw Value; // error
            return AttributeParser::Mapify(Value);
        }
    };

    class DOM
    {
        DOM *parent;
        tstring elementName;
        vector<unique_ptr<DOM>> elements;
        unique_ptr<AttributeMap> attributes;
        tstring value;

    public:
        DOM * const &Parent;
        DOM(const tstring &elementName) : parent(0), elementName (elementName), Parent(parent) { }
        DOM() : parent(0), Parent(parent) { }
        void Add(unique_ptr<DOM> e)
        {
            e->parent = this;
            elements.push_back(std::move(e));
        }
        void Put(unique_ptr<AttributeMap> a) { attributes = std::move(a); }
        const tstring &GetElementName() { return elementName; }
        bool IsElement(const tstring &name,  DOM *&pd); // deprecated, do not use
        DOM *GetElement(const tstring &name)
        {
            const auto end = elements.cend();
            for (auto it = elements.cbegin(); it != end; ++it)
            {
                if ((*it)->elementName == name)
                {
                    return &**it;
                }
            }
            return nullptr;
        }
        vector<std::reference_wrapper<DOM>> AllElements();
        tstring *GetAttribute(const tstring &name)
        {
            if (!attributes.get()) return nullptr;
            const auto it = attributes->find(name);
            if (it == attributes->end()) return nullptr;
            return &it->second;
        }
        AttributeMap *GetAttributes()
        {
            return attributes.get();
        }
        void SetValue(const tstring &val)
        {
            if (!StringUtils::Ltrim(val).empty())
            {
                if (!elementName.empty()) throw _T("A DOM node can only have a value or an element name not both!");
                value = val;
            }
        }
        const tstring &GetValue()
        {
            if (elementName.empty()) // a text node
            {
                return value;
            }
            else if (elements.size() == 1) // a single child node
            {
                const auto &e = elements.back();
                if (e->elementName.empty()) return e->value; // and that's a text node
            }
            throw _T("This operation can be called only for a text node or a node whose ONLY child is a text node!");
        }
    };

    struct IParser
    {
        virtual bool NextToken() = 0;
        virtual const TokenResult &GetToken() = 0;
        virtual const tstring &GetElement() = 0;
        virtual bool SkipToStartTag(bool notBeyondEndTag = true) = 0;
        virtual bool SkipToEndTag() = 0;
        virtual unique_ptr<DOM> Domify(bool skipWhiteBody = true) = 0;
        virtual void SaveState() = 0;
        virtual void RestoreState() = 0;
    };

    class LIB_EXPORTS Parser : public IParser
    {
        std::ifstream xmlfile;
        std::streampos cursor;
        TokenResult tr, savedTr;
        std::stack<tstring> startedTags, savedTags;
        size_t lineNo, savedLineNo;

    public:
        const size_t &LineNo;
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
        unique_ptr<DOM> Domify(bool skipWhiteBody = true);
        void SaveState() { cursor = xmlfile.tellg(); savedTr = tr; savedLineNo = lineNo; savedTags = startedTags; }
        void RestoreState() { xmlfile.seekg(cursor); tr = savedTr; lineNo = savedLineNo; startedTags = savedTags; }
        ~Parser();
    };
}

