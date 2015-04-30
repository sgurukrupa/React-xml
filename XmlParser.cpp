#include "XmlParser.h"
#include "XmlAttributeParser.h"

using cpputils::tstring;
using Xml::Parser;

Parser::Parser(const tstring &fn)
    : xmlfile(fn), tr(), lineNo(1)
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
    enum StringDelimiter : char { SINGLE_QUOTE = '\'', DOUBLE_QUOTE = '"' } stringDelim;

    TOKEN_TYPE toktype = tr.Type == START_TAG && !tr.EndOfStartTag ? ATTRIBUTE : BODY; // start TAG is kinda odd, it encapsulates another kind of token inside
    tstring tokenBuf;
    auto addtok = [&tokenBuf](char ch) { tokenBuf.append(1, ch); };
    for (char c; xmlfile.get(c);)
    {
        if (c == '\n') ++lineNo;
        if (c == '<')
        {
            switch (toktype)
            {
            default:  // </hari><
                throw "Encountered error while parsing XML."; // error
            case BODY:
                if (!tokenBuf.empty())
                {
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    xmlfile.unget();
                    return true;
                }
                if (!xmlfile.get(c) || isspace(c)) throw "Encountered error while parsing body."; // to determine if this is an END_TAG or START_TAG, read one more character from file
                if (c == '/')
                {
                    if (!xmlfile.get(c) || isspace(c)) throw "Encountered error while parsing body."; // an XML error - a '</' must not be followed by spaces
                    toktype = END_TAG;
                    xmlfile.unget();
                }
                else if (c == '!')
                {
                    if (!xmlfile.get(c) || c != '-' || !xmlfile.get(c) || c != '-') throw "Invalid XML comment!";
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
                throw "Encountered error while parsing body."; // error
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
                if (tokenBuf.empty()) throw "Empty end tag."; // end tag is empty
                if (startedTags.top() != tokenBuf) throw "Start tag doesn't match end tag."; // end tag doens't match start Tag
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
                if (!xmlfile.get(c) || c != '>') throw "Encountered error while parsing start tag."; // error
                if (toktype == START_TAG)
                {
                    startedTags.push(tokenBuf);
                }
                tr.NoBody = tr.EndOfStartTag = true;
                tr.Value = tokenBuf;
                tr.Type = toktype;
                return true;
            case END_TAG:
                throw "Encountered error while parsing end tag."; // error - </*/
            }
        }
        else if (c == '-')
        {
            switch (toktype)
            {
            case COMMENT:
                char c2;
                if (xmlfile.get(c2) && c2 == '-')
                {
                    if (!xmlfile.get(c2) || c2 != '>') throw "Invalid XML comment!";
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    return true;
                }
                else if (xmlfile.eof()) throw "Invalid XML comment!";
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
                throw "Encountered error while parsing end tag."; // error no spaces allowed
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
                char c2;
                if (xmlfile.get(c2) && c2 == '>')
                {
                    tr.Value = tokenBuf;
                    tr.Type = toktype;
                    return true;
                }
                else if (xmlfile.eof()) throw "Invalid XML processing instruction.";
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
    if (startedTags.size() == 0) throw ""; // error
    return startedTags.top();
}

bool Parser::SkipToStartTag(bool notBeyondEndTag)
{
    //auto parent = GetElement();
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

Parser::~Parser(void)
{
    xmlfile.close();
}
