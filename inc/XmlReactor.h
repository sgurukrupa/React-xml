#pragma once
#include "CppUtils.h"
#include "XmlParser.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Xml
{
    using cpputils::tstring;
    using std::vector;
    using std::map;
    using std::unique_ptr;

    class ElementAttributes
    {
        std::unique_ptr<const map<tstring, tstring>> pattrs;
    public:
        const tstring Name;
        const map<tstring, tstring> &Attributes;
        ElementAttributes(const tstring &name, std::unique_ptr<const AttributeMap> pattributes)
            : pattrs(std::move(pattributes)), Name(name), Attributes(*pattrs)
        { }
        ElementAttributes(ElementAttributes &&e)
            : pattrs(std::move(e.pattrs)), Name(e.Name), Attributes(*pattrs)
        { }
        bool IsAttribute(const tstring &attr, tstring &value) const
        {
            const auto it = Attributes.find(attr);
            if (it == Attributes.end()) return false;
            value = it->second;
            return true;
        }
    };

    class LIB_EXPORTS Reactor
    {
        vector<ElementAttributes> elementStack;
        const tstring getCurrentPath();
        bool xPathEquals(const tstring &eventPath);
        void callHandlersIfPathsMatch();

    public:
        struct Event
        {
            Reactor &Source;
            Event (Reactor &ev) : Source(ev) { }
            const ElementAttributes &GetElement(int level)
            {
                const auto len = Source.elementStack.size();
                if (len == 0) throw _T("Element stack is empty!");
                return Source.elementStack.at(len - 1 - level);
            }
            const tstring &Attribute(const tstring &attrKey, int level = 0)
            {
                return GetElement(level).Attributes.at(attrKey);
            }
            bool IsAttribute(const tstring &attr, tstring &value, int level = 0)
            {
                return GetElement(level).IsAttribute(attr, value);
            }
            unique_ptr<DOM> DomifySelf(bool skipWhiteBody = true)
            {
                Source.Parser.SaveState();
                const auto &e = Source.elementStack.back();
                DOM *root = new DOM(e.Name);
                root->Put(unique_ptr<AttributeMap>(new AttributeMap(e.Attributes)));
                for (unique_ptr<DOM> d; (d = Source.Parser.Domify(skipWhiteBody)).get();)
                {
                    root->Add(std::move(d));
                }
                Source.Parser.RestoreState();
                return unique_ptr<DOM>(root);
            }
        };

        IParser &Parser;
        const tstring GlobalEnablingAttribute;
        Reactor(IParser &xp, const tstring &enablingAttribute = _T("enabled"));
        typedef void (*EventHandler)(Event);
        void AddEvent(const tstring &eventPath, EventHandler);
        void Run(bool runToEnd = false); // start capturing events
        void AddPreHook(EventHandler);

    private:
        map<tstring, vector<EventHandler>> handlerMap;
        vector<EventHandler> preHooks;
    };
}

