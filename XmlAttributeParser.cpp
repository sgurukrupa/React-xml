#include "XmlAttributeParser.h"
#include "StringUtils.h"

using cpputils::tstring;
using Xml::AttributeParser;

AttributeParser::AttributeParser(const tstring& allattr)
    : attr(allattr)
{ }

bool AttributeParser::NextToken()
{
    attr = StringUtils::Ltrim(attr);
    if (attr.empty()) return false;
    auto i = attr.find('=');
    if (i == tstring::npos) throw _T("Invalid XML attribute");
    Key = attr.substr(0, i);
    attr = StringUtils::Ltrim(attr.substr(i + 1)); // it's valid if i == attr.size()
    if (attr.empty()) throw _T("Invalid XML attribute");
    if (attr[0] == '"')
    {
        i = attr.find('"', 1);
    }
    else if (attr[0] == '\'')
    {
        i = attr.find('\'', 1);
    }
    else throw _T("Invalid XML attribute");
    if (i == tstring::npos) throw _T("Invalid XML attribute");
    Value = attr.substr(1, i - 1);
    attr.erase(0, i + 1);
    return true;
}
