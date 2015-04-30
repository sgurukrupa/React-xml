#include "XmlReactor.h"
#include "StringUtils.h"
#include <algorithm>

using Xml::Eventer;
using cpputils::tstring;

Eventer::Eventer(Xml::IParser &xp)
    : xp(xp)
{ }

void Eventer::AddEvent(const tstring &eventPath, EventHandler eh)
{
    auto ep = StringUtils::Ltrim(eventPath); // left trimming is sufficient!
    if (ep.empty()) throw "Empty xpath expression!";
    handlerMap[ep].push_back(eh);
}

//template <class T> static const tstring GetXPath(T start, T end)
//{
//    if (start > end) throw _T("");
//    tstring s;
//    for (T it = start; it != end; ++it)
//    {
//        s.append(*it);
//        if (it + 1 != end) s.append(_T("/"));
//    }
//    return s;
//}

const tstring Eventer::getCurrentPath()
{
    tstring s;
    const auto end = elementStack.cend();
    for (auto it = elementStack.cbegin(); it != end; ++it)
    {
        s.append(_T("/")).append(it->Name);
    }
    return s;
}

bool Eventer::xPathEquals(const tstring &eventPath)
{
    StringUtils::Tokenizer ept(eventPath, _T("/"));
    StringUtils::Tokenizer cpt(getCurrentPath(), _T("/"));
    if (eventPath[0] != _T('/')) // relative path !
    {
        ept.NextToken(); // the first token!
        bool cN;
        while ((cN = cpt.NextToken()) && cpt.GetToken() != ept.GetToken())
            ; // no body !
        if (!cN) return false;
    }
    while (ept.NextToken())
    {
        if (!cpt.NextToken()) return false;
        if (ept.GetToken() != cpt.GetToken()) return false; // the first token of both are always empty, the real test begins in second iteration
    }
    return !cpt.NextToken();
}

void Eventer::callHandlers()
{
    const auto end = handlerMap.cend();
    for (auto it = handlerMap.cbegin(); it != end; ++it)
    {
        if (xPathEquals(it->first))
        {
            Event e;
            for (auto i = elementStack.size(); i > 0; --i)
            {
                e.AttributeMaps.push_back(
            }
            std::for_each(it->second.begin(), it->second.end(), [&e](EventHandler eh) { eh(e); });
        }
    }
}

void Eventer::Run()
{
    while (xp.NextToken())
    {
        switch (xp.GetToken().Type)
        {
        case START_TAG:
            if (xp.GetToken().EndOfStartTag)
            {
                elementStack.push_back(ElementAttributes(xp.GetToken().Value, std::unique_ptr<map<tstring, tstring>>(new map<tstring, tstring>())));
            }
            else
            {
                auto elem = xp.GetToken().Value;
                if (xp.NextToken())
                {
                    elementStack.push_back(ElementAttributes(elem, xp.GetToken().Attributify()));
                }
            }
            callHandlers();
            break;
        case END_TAG:
            if (elementStack.empty()) throw _T("Invalid XML!");
            if (!xp.GetToken().Value.empty() && xp.GetToken().Value != elementStack.back().Name) throw _T("XML element has no closing tag!");
            elementStack.pop_back();
            break;
        }
    }
}
