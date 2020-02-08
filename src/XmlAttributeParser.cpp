#include "XmlAttributeParser.h"

#include <utility>
#include "StringUtils.h"
#include "XmlException.h"

using reactxml::cpputils::tstring;
using reactxml::AttributeParser;

AttributeParser::AttributeParser (tstring allattr)
    : attr (std::move(allattr))
{ }

bool AttributeParser::NextToken()
{
    attr = StringUtils::Ltrim(attr);
    if (attr.empty()) return false;
    auto i = attr.find('=');
    if (i == tstring::npos) throw Exception("Invalid XML attribute");
    Key = StringUtils::Rtrim(attr.substr(0, i));
    attr = StringUtils::Ltrim(attr.substr(i + 1)); // it's valid if i == attr.size(), Haribol ..
    if (attr.empty()) throw Exception("Invalid XML attribute");
    if (attr[0] == '"')
    {
        i = attr.find('"', 1);
    }
    else if (attr[0] == '\'')
    {
        i = attr.find('\'', 1);
    }
    else throw Exception("Invalid XML attribute");
    if (i == tstring::npos) throw Exception("Invalid XML attribute");
    Value = attr.substr(1, i - 1);
    attr.erase(0, i + 1);
    return true;
}

