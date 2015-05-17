#include "XmlParser.h"
#include <iostream>
int main()
{
  auto x = Xml::Parser("sample.xml");
  while (x.NextToken())
  std::cout << x.GetToken().Value << std::endl;
}
