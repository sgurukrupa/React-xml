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
  //auto x = Xml::Parser(_T("sample.xml"));
  //std::wofstream cout(_T("output.txt"));
  //while (x.NextToken())
  //{
  //  cout << x.GetToken().Value << std::endl;
  //}
  //StringUtils::GroupTokenizer x(_T("Hari said \"All you souls please come ..\" and left for Vaikuntha."), StringUtils::WHITESPACE, std::make_pair(_T('"'), _T('"'))); 
  StringUtils::GroupTokenizer x(_T("Hari said sssh\"\"and left for Vaikuntha."), StringUtils::WHITESPACE, std::make_pair(_T('"'), _T('"'))); 
  //StringUtils::Tokenizer x(_T("Hari said \"All you souls please come ..\" and left for Vaikuntha."), StringUtils::WHITESPACE); 
  while (x.NextToken()) std::cout << to_str(x.GetToken()) << std::endl;
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
  if (x.NextToken())
  {
    std::cout << to_str(x.GetToken()) << std::endl;
  }
  else
  {
    std::cout << "[No token]" << std::endl;
  }
}
