#include "XmlReactor.h"
#include "StringUtils.h"
#include <algorithm>

using reactxml::cpputils::tstring;
using reactxml::Reactor;

Reactor::Reactor(reactxml::IParser &xp, const tstring &enablingAttribute)
    : Parser(xp), GlobalEnablingAttribute(enablingAttribute)
{ }

void Reactor::AddEvent(const tstring &eventPath, EventHandler eh)
{
    auto ep = StringUtils::Ltrim(eventPath); // left trimming is sufficient!
    if (ep.empty()) throw Exception("Empty xpath expression!");
    handlerMap[ep].push_back(eh);
}

const tstring Reactor::getCurrentPath()
{
    tstring s;
    const auto end = elementStack.cend();
    for (auto it = elementStack.cbegin(); it != end; ++it)
    {
        s.append(_T("/")).append(it->Name);
    }
    return s;
}

bool Reactor::xPathEquals(const tstring &eventPath)
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

void Reactor::callHandlersIfPathsMatch()
{
    { // localized block
        tstring enabled;
        if (elementStack.back().IsAttribute(GlobalEnablingAttribute, enabled) && enabled == _T("no")) return;
    }
    const auto end = handlerMap.cend();
    for (auto it = handlerMap.cbegin(); it != end; ++it)
    {
        if (xPathEquals(it->first))
        {
            Event e(*this);
            std::for_each(preHooks.cbegin(), preHooks.cend(), [&e](EventHandler eh) { eh(e); });
            std::for_each(it->second.begin(), it->second.end(), [&e](EventHandler eh) { eh(e); });
        }
    }
}

void Reactor::Run(const bool runToEnd)
{
    while (Parser.NextToken())
    {
        switch (Parser.GetToken().Type)
        {
        case START_TAG:
            if (Parser.GetToken().EndOfStartTag)
            {
                elementStack.push_back(ElementAttributes(Parser.GetToken().Value, unique_ptr<AttributeMap>(new AttributeMap())));
            }
            else
            {
                auto elemName = Parser.GetToken().Value;
                if (Parser.NextToken()) // must be an attribute ..
                {
                    elementStack.push_back(ElementAttributes(elemName, Parser.GetToken().Attributify()));
                }
            }
            callHandlersIfPathsMatch();
            break;
        case END_TAG:
            if (!elementStack.empty())
            {
                elementStack.pop_back();
            }
            else if (!runToEnd) throw Exception("Invalid XML!");
            break;
        }
    }
}

void Reactor::AddPreHook(EventHandler eh)
{
    preHooks.push_back(eh);
}

