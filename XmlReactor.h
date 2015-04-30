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

    struct Event
    {
        class MapBox
        {
            const map<tstring, tstring> *const pMap;
        public:
            MapBox(map<tstring, tstring> *pm)
                : pMap(pm)
            { }
            const tstring &at(const tstring &attr)
            {
                if (!pMap) throw pMap;
                return pMap->at(attr);
            }
        };
        vector<MapBox> AttributeMaps; // [0] => current element, [1] => parent element and so on.
    };

    class LIB_EXPORTS Eventer
    {
        IParser &xp;
        class ElementAttributes
        {
            std::unique_ptr<const map<tstring, tstring>> pattrs;
        public:
            const tstring Name;
            const map<tstring, tstring> &Attributes;
            ElementAttributes(const tstring &name, std::unique_ptr<const map<tstring, tstring>> pattributes)
                : pattrs(std::move(pattributes)), Name(name), Attributes(*pattrs)
            { }
            //ElementAttributes(const tstring &name, map<tstring, tstring> *pattributeMap)
            //    : this(name, std::unique_ptr<const map<tstring, tstring>>(pattributeMap))
            //{ }
            ElementAttributes(ElementAttributes &&e)
                : pattrs(std::move(e.pattrs)), Name(e.Name), Attributes(*pattrs)
            { }
        };
        vector<ElementAttributes> elementStack;
        const tstring getCurrentPath();
        bool xPathEquals(const tstring &eventPath);
        void callHandlers();

    public:
        Eventer(IParser &xp);
        typedef void (*EventHandler)(Event);
        void AddEvent(const tstring &eventPath, EventHandler);
        void Run(); // start capturing events

    private:
        map<tstring, vector<EventHandler>> handlerMap;
    };
}
