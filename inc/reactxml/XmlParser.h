#pragma once
#include <fstream>
#include <string>
#include <stack>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <functional>
#include <cassert>
#include "CppUtils.h"
#include "XmlAttributeParser.h"
#include "StringUtils.h"
#include "XmlException.h"

namespace reactxml
{
    enum TOKEN_TYPE { BODY, START_TAG, ATTRIBUTE, END_TAG, COMMENT, INSTRUCTION }; // ATTRIBUTE is more specific case of START_TAG, Haribol ..

    using cpputils::tstring;
    using std::map;
    using std::unique_ptr;
    using std::vector;

    struct LIB_EXPORTS TokenResult
    {
        tstring Value;
        TOKEN_TYPE Type { BODY };
        bool EndOfStartTag { false }; // only applicable when Type == START_TAG or ATTRIBUTE, Haribol ..
        bool NoBody { false }; // only applicable when EndOfStartTag is true, Haribol ..

        std::unique_ptr<AttributeMap> Attributify() const
        {
            if (Type != ATTRIBUTE) throw Exception("The current token is not an attribute."); // error, Haribol
            return AttributeParser::Mapify(Value);
        }
    };

    class DOM
    {
      DOM *parent { nullptr };
      const tstring elementName;
      vector<unique_ptr<DOM>> elements;
      unique_ptr<AttributeMap> attributes;
      tstring value;

      tstring stringAttributes()
      {
        tstring str;

        if (attributes)
        {
          for (const auto& attr : *attributes)
          {
            str += " " + attr.first + "=" + "\"" + attr.second + "\"";
          }
        }

        return str;
      }

    public:
        DOM() = default;

        DOM (tstring elementName)
          : elementName (std::move(elementName))
        { }

        DOM* getParent()
        {
          return parent;
        }

        DOM* insert (unique_ptr<DOM>&& child)
        {
          assert(!elementName.empty());
          assert(!child->parent);
          child->parent = this;
          const auto pchild = child.get(); // save for later use, Haribol ..
          elements.push_back(std::move(child));
          return pchild;
        }

        void insertElements (std::vector<unique_ptr<DOM>>&& children)
        {
          for (auto& child : children)
          {
            insert(std::move(child));
          }
        }

        DOM* insert (const tstring& childName)
        {
          return insert(std::make_unique<DOM>(childName));
        }

        std::unique_ptr<DOM> remove (DOM* child)
        {
          for (auto it = elements.begin(); it != elements.end(); ++it)
          {
            if (it->get() == child)
            {
              auto child = std::move(*it);
              child->parent = nullptr;
              elements.erase(it);
              return child;
            }
          }

          throw Exception("The child DOM wasn't found!");
        }

        std::vector<std::unique_ptr<DOM>> removeElements()
        {
          for (const auto& element : elements)
          {
            element->parent = nullptr;
          }

          return std::move(elements);
        }

        void setAttribute (const std::string& name, const std::string& value)
        {
          assert(!elementName.empty());

          if (!attributes)
          {
            attributes = std::make_unique<AttributeMap>();
          }

          (*attributes)[name] = value;
        }

        void Put (unique_ptr<AttributeMap> a) { attributes = std::move(a); }
        const tstring &GetElementName() { return elementName; }

        DOM *getElement (const tstring &name, bool throwOnUnableToGet = true)
        {
            const auto end = elements.cend();
            for (auto it = elements.cbegin(); it != end; ++it)
            {
                if ((*it)->elementName == name)
                {
                    return &**it;
                }
            }
            
            if (throwOnUnableToGet) throw Exception("Unable to get element '" + name + "'.");

            return nullptr;
        }

        vector<std::reference_wrapper<DOM>> AllElements();

        tstring *getAttribute (const tstring &name, bool throwOnUnableToGet = true)
        {
            if (attributes)
            {
                const auto it = attributes->find(name);

                if (it != attributes->end())
                {
                    return &it->second;
                }
            }

            if (throwOnUnableToGet) throw Exception("Unable to get attribute '" + name + "'.");

            return nullptr;
        }

        AttributeMap *GetAttributes()
        {
            return attributes.get();
        }

        void setValue (const tstring &val)
        {
            if (!StringUtils::Ltrim(val).empty())
            {
                //if (!elementName.empty()) throw Exception("A DOM node can only have a value or an element name not both!");
                if (elementName.empty())
                {
                  value = val;
                }
                else if (elements.empty())
                {
                  auto elem = insert(std::make_unique<DOM>());
                  elem->value = val;
                }
                else if (elements.size() == 1 && elements[0]->elements.empty())
                {
                  elements[0]->value = val;
                }
                else
                {
                  throw Exception("Cannot set value for a nested element!");
                }
            }
        }

        tstring *getValue (bool throwOnUnableToGet = true)
        {
            if (elementName.empty()) // a text node, Haribol
            {
                return &value;
            }

            if (elements.size() == 1) // a single child node, Haribol
            {
                const auto &e = elements.back();
                if (e->elementName.empty()) return &e->value; // and that's a text node, Haribol
            }

            if (throwOnUnableToGet)
            {
              throw Exception("This operation can be called only for a text node or a node whose ONLY child is a text node!");
            }

            return nullptr;
        }

        tstring toString()
        {
          tstring str;
          str = elementName.empty() ? value :
            "<" + elementName + stringAttributes() + (elements.empty() ? " />" : ">");

          if (!elements.empty())
          {
            for (const auto& e : elements)
            {
              str += e->toString();
            }

            str += "</" + elementName + ">";
          }

          return str;
        }

        std::unique_ptr<DOM> clone()
        {
          auto root = std::make_unique<DOM>(elementName);

          for (const auto& e : elements)
          {
            root->elements.push_back(e->clone());
          }

          if (attributes)
          {
            root->attributes = std::make_unique<AttributeMap>(*attributes);
          }

          root->value = value;
          return root;
        }
    };

    struct IParser
    {
        virtual bool NextToken() = 0;
        virtual const TokenResult &GetToken() = 0;
        virtual const tstring &GetElement() = 0;
        virtual bool SkipToStartTag(bool notBeyondEndTag = true) = 0;
        virtual bool SkipToEndTag(bool matchStartTag = false) = 0;
        virtual unique_ptr<DOM> Domify(bool skipWhiteBody = true) = 0;
        virtual void SaveState() = 0;
        virtual void RestoreState() = 0;
    };

    class LIB_EXPORTS Parser : public IParser
    {
        cpputils::tifstream xmlfile;
        std::streampos cursor;
        TokenResult tr, savedTr;
        std::stack<tstring> startedTags, savedTags;
        size_t lineNo, savedLineNo;

    public:
        Parser (const Parser&) = delete;
        Parser& operator = (const Parser&) = delete;
        Parser (Parser&&) = delete;
        Parser& operator = (Parser&&) = delete;

        Parser (const tstring &filename);

        const size_t &LineNo;
        bool NextToken() override;
        const TokenResult &GetToken() override;
        const tstring &GetElement() override;
        bool SkipToStartTag(bool notBeyondEndTag = true) override;
        bool SkipToEndTag(bool matchStartTag = false) override;
        unique_ptr<DOM> Domify(bool skipWhiteBody = true) override;

        void SaveState() override
        {
          cursor = xmlfile.tellg(); savedTr = tr; savedLineNo = lineNo; savedTags = startedTags;
        }

        void RestoreState() override
        {
          xmlfile.seekg(cursor); tr = savedTr; lineNo = savedLineNo; startedTags = savedTags;
        }

        ~Parser();
    };
}

