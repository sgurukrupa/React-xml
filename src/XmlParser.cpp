#include "XmlParser.h"
#include "XmlAttributeParser.h"

using cpputils::tstring;
using Xml::Parser;

Parser::Parser(const tstring &fn)
    : xmlfile(fn), cursor(xmlfile.tellg()), tr(), savedTr(), lineNo(1), savedLineNo(1), LineNo(lineNo)
{ }

bool Parser::NextToken()
{
    if ((tr.Type == START_TAG || tr.Type == ATTRIBUTE) && tr.NoBody)
    {
        startedTags.pop();
        tr.Value = _T("");
        tr.Type = END_TAG;
        return true;
    }

    auto startTagDone = false;
    auto insideString = false;
    enum StringDelimiter : TCHAR { SINGLE_QUOTE = _T('\''), DOUBLE_QUOTE = _T('"') } stringDelim;

    TOKEN_TYPE toktype = tr.Type == START_TAG && !tr.EndOfStartTag ? ATTRIBUTE : BODY; // start TAG is kinda odd, it encapsulates another kind of token inside
    tstring tokenBuf;
    auto addtok = [&tokenBuf](TCHAR ch) { tokenBuf.append(1, ch); };
    for (TCHAR c; xmlfile.get(c);)
    {
        if (c == '\n') ++lineNo;
        if (c == '<')
        {
            switch (toktype)
            {
            default:  // </hari><
                throw Exception("Encountered error while parsing XML."); // error
            case BODY:
                if (!tokenBuf.empty())
                {
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    xmlfile.unget();
                    return true;
                }
                if (!xmlfile.get(c) || isspace(c)) throw Exception("Encountered error while parsing body."); // to determine if this is an END_TAG or START_TAG, read one more character from file
                if (c == '/')
                {
                    if (!xmlfile.get(c) || isspace(c)) throw Exception("Encountered error while parsing body."); // an XML error - a '</' must not be followed by spaces
                    toktype = END_TAG;
                    xmlfile.unget();
                }
                else if (c == '!')
                {
                    if (!xmlfile.get(c) || c != '-' || !xmlfile.get(c) || c != '-') throw Exception("Invalid XML comment!");
                    toktype = COMMENT;
                }
                else if (c == '?')
                {
                    toktype = INSTRUCTION;
                }
                else
                {
                    toktype = START_TAG;
                    xmlfile.unget();
                }
                break;
            }
        }
        else if (c == '>')
        {
            switch (toktype)
            {
            case BODY:
                throw Exception("Encountered error while parsing body."); // error
            case ATTRIBUTE:
                if (insideString)
                {
                    addtok(c);
                    break;
                }
            case START_TAG:
                if (toktype == START_TAG) startedTags.push(tokenBuf);
                tr.NoBody = false;
                tr.EndOfStartTag = true;
                tr.Value = tokenBuf;
                tr.Type = toktype;
                return true;
            case END_TAG:
                if (tokenBuf.empty()) throw Exception("Empty end tag."); // end tag is empty
                if (startedTags.top() != tokenBuf) throw Exception("Start tag doesn't match end tag."); // end tag doens't match start Tag
                startedTags.pop();
                tr.Value = tokenBuf;
                tr.Type = toktype;
                return true;
            default:
                addtok(c);
                break;
            }
        }
        else if (c == '/')
        {
            switch (toktype)
            {
            default:
                addtok(c);
                break;
            case ATTRIBUTE:
                if (insideString)
                {
                    addtok(c);
                    break;
                }
            case START_TAG:
                if (!xmlfile.get(c) || c != '>') throw Exception("Encountered error while parsing start tag."); // error
                if (toktype == START_TAG)
                {
                    startedTags.push(tokenBuf);
                }
                tr.NoBody = tr.EndOfStartTag = true;
                tr.Value = tokenBuf;
                tr.Type = toktype;
                return true;
            case END_TAG:
                throw Exception("Encountered error while parsing end tag."); // error - </*/
            }
        }
        else if (c == '-')
        {
            switch (toktype)
            {
            case COMMENT:
                TCHAR c2;
                if (xmlfile.get(c2) && c2 == '-')
                {
                    if (!xmlfile.get(c2) || c2 != '>') throw Exception("Invalid XML comment!");
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    return true;
                }
                else if (xmlfile.eof()) throw Exception("Invalid XML comment!");
                xmlfile.unget(); // unget c2
            default:
                addtok(c);
                break;
            }
        }
        else if (isspace(c))
        {
            switch (toktype)
            {
            case START_TAG:
                startTagDone = true; // waiting for the attributes to follow ..
                break;
            default:
                addtok(c);
                break;
            case END_TAG:
                throw Exception("Encountered error while parsing end tag."); // error no spaces allowed
                break;
            }
        }
        else if (c == '\'' || c == '"')
        {
            switch(toktype)
            {
            case ATTRIBUTE:
                if (insideString && stringDelim == c)
                {
                    insideString = false;
                }
                else if (!insideString)
                {
                    insideString = true;
                    stringDelim = static_cast<StringDelimiter>(c);
                }
            default:
                addtok(c);
                break;
            }
        }
        else if (c == '?')
        {
            switch (toktype)
            {
            case INSTRUCTION:
                TCHAR c2;
                if (xmlfile.get(c2) && c2 == '>')
                {
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    return true;
                }
                else if (xmlfile.eof()) throw Exception("Invalid XML processing instruction.");
                xmlfile.unget();
            default:
                addtok(c);
                break;
            }
        }
        else
        {
            switch (toktype)
            {
            case START_TAG:
                if (startTagDone) // oops! bumped into attributes ..
                {
                    startedTags.push(tokenBuf);
                    tr.NoBody = tr.EndOfStartTag = false;
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    xmlfile.unget();
                    return true;
                }
            default:
                addtok(c);
                break;
            }
        }
    } // end for
    return false;
}

const Xml::TokenResult &Parser::GetToken()
{
    return tr;
}

const tstring &Parser::GetElement()
{
    if (startedTags.size() == 0) throw Exception(""); // error
    return startedTags.top();
}

bool Parser::SkipToStartTag(bool notBeyondEndTag)
{
    const auto depth = startedTags.size();
    while (NextToken())
    {
        switch (GetToken().Type)
        {
        case START_TAG:
            return true;
        case END_TAG:
            if (notBeyondEndTag && startedTags.size() < depth) return false;
            break;
        default:
            break;
        }
    }
    return false;
}

bool Parser::SkipToEndTag(bool matchStartTag)
{
    const auto depth = startedTags.size();
    while (NextToken())
    {
        switch (GetToken().Type)
        {
        case END_TAG:
            if (!matchStartTag) return true;
            if (startedTags.size() < depth) return true;
            break;
        default:
            break;
        }
    }
    return false;
}

using std::unique_ptr;
unique_ptr<Xml::DOM> Parser::Domify(bool skipWhiteBody)
{
    DOM *root, *d = 0; // 'd' => The(D) insertion point
    const auto startSize = startedTags.size();
    while (NextToken())
    {
        switch (GetToken().Type)
        {
        case START_TAG:
            { // localize block
                auto p = new DOM(GetToken().Value);
                if (startedTags.size() == startSize + 1)
                {
                    root = d = p; // the beginning of creation ..!
                }
                else // must be greater (cannot be smaller)
                {
                    d->Add(unique_ptr<DOM>(p));
                    d = p; // the next insertion point
                }
            }
            break;
        case ATTRIBUTE:
            if (!d) throw Exception("DOM cannot start at an ATTRIBUTE");
            d->Put(GetToken().Attributify());
            break;
        case BODY:
            if (startedTags.size() > startSize) // skip the beginning BODY or trailing BODY
            {
                auto &val = GetToken().Value;
                if (StringUtils::Ltrim(val).empty() && skipWhiteBody) break;
                auto p = new DOM();
                p->SetValue(GetToken().Value);
                d->Add(unique_ptr<DOM>(p));
            }
            break;
        case END_TAG:
            if (startedTags.size() < startSize) // only possible when the parser is positioned on and END tag when Domify was called
            {
                return unique_ptr<DOM>();
            }
            if (startedTags.size() == startSize)
            {
                return unique_ptr<DOM>(root);
            }
            else
            {
                d = d->Parent;
            }
            break;
        default:
            break;
        }
    }
    throw Exception("Invalid XML - unclosed tags!");
}

using Xml::DOM;
/*
bool DOM::IsElement(const tstring &name, DOM *&pd)
{
    const auto end = elements.cend();
    for (auto it = elements.cbegin(); it != end; ++it)
    {
        if ((*it)->elementName == name)
        {
            pd = &**it;
            return true;
        }
    }
    return false;
}
*/

std::vector<std::reference_wrapper<DOM>> DOM::AllElements()
{
    vector<std::reference_wrapper<DOM>> v;
    const auto end = elements.cend();
    for (auto it = elements.cbegin(); it != end; ++it) v.push_back(std::ref(**it));
    return v;
}

Parser::~Parser(void)
{
    xmlfile.close();
}

