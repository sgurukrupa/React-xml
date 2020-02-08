#include <reactxml/XmlParser.h>
#include <iostream>

int main()
{
  using namespace reactxml;
  reactxml::Parser x("device_config.xml");
  const auto dom = x.Domify();
  const auto stage = dom->getElement("Stage");
  std::cout << "So far so good" << std::endl;
  //std::cout << *stage->getValue("AdapterName") << std::endl;
  std::cout << dom->toString() << std::endl;
  auto nd = DOM("nextD");
  nd.insert(dom->remove(stage));
  std::cout << "after removing .. " << std::endl;
  std::cout << dom->toString() << std::endl;
  std::cout << "next Dom .." << std::endl;
  std::cout << nd.toString() << std::endl;
  dom->insert(stage->clone());
  std::cout << "after cloning .. " << std::endl;
  std::cout << dom->toString() << std::endl;
  auto dev2 = DOM("device2");
  dev2.insertElements(dom->getElement("Stage")->removeElements());
  std::cout << "after removing array of elements .." << std::endl;
  std::cout << dom->toString() << std::endl;
  std::cout << "new array .." << std::endl;
  std::cout << dev2.toString() << std::endl;
}
