cmake_minimum_required(VERSION 3.13)

project(reactxml
  VERSION 3.0.0
  LANGUAGES CXX)

add_library(reactxml SHARED
  src/XmlAttributeParser.cpp
  src/StringUtils.cpp
  src/XmlParser.cpp
  inc/reactxml/XmlReactor.h
  inc/reactxml/XmlAttributeParser.h
  inc/reactxml/StringUtils.h
  inc/reactxml/XmlParser.h
  inc/reactxml/CppUtils.h
  inc/reactxml/XmlException.h)

target_include_directories(reactxml
  PRIVATE inc/reactxml
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>)

include(GNUInstallDirs)

install(DIRECTORY inc/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS reactxml
  EXPORT reactxml
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT reactxml
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/reactxml
  FILE reactxmlConfig.cmake)
