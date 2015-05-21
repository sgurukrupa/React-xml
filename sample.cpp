#include "XmlParser.h"
#include <iostream>

std::string to_str(const cpputils::tstring &s)
{
  std::string r;
  for (auto it = s.begin(); it != s.end(); ++it)
  {
    r.append(1, *it);
  }
  return r;
}

int main()
{
  auto x = Xml::Parser(_T("sample.xml"));
  std::wofstream cout(_T("output.txt"));
  while (x.NextToken())
  {
    cout << x.GetToken().Value << std::endl;
  }
}
