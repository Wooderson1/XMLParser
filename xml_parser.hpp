/*
    xml_parser.hpp

    Declaration file for xml parsing
*/

#ifndef INCLUDED_XML_PARSER_HPP
#define INCLUDED_XML_PARSER_HPP
#include <string>
#include <algorithm>

// check if declaration
bool isXMLDeclaration(std::string::const_iterator pc);

// check if end tag
bool isXMLEndTag(std::string::const_iterator pc);

// check if start tag
bool isXMLStartTag(std::string::const_iterator pc);

// check if namespace
bool isXMLNamespace(std::string::const_iterator& pc, std::string::const_iterator endpc,  bool intag);

// check if attribute
bool isXMLAttribute(std::string::const_iterator pc, bool intag);

// check if CDATA
bool isXMLCData(std::string::const_iterator pc);

// check if comment
bool isXMLComment(std::string::const_iterator pc);

// check if characters before XML
bool isBeforeXML(std::string::const_iterator pc, int depth);

// check if entity
bool isXMLEntity(std::string::const_iterator pc);

// check if characters
bool isXMLCharacters(std::string::const_iterator pc);

// parse xml declaration
std::string::const_iterator parseXMLDeclaration(std::string::const_iterator pc, std::string& buffer, long& total);

// parse required xml version
std::string::const_iterator parseXMLVersion(std::string::const_iterator pc,  std::string& buffer, std::string::const_iterator& pnameend, std::string::const_iterator& pvalueend);

// parse encoding
std::string::const_iterator parseXMLEncoding(std::string::const_iterator pc,  std::string& buffer, std::string::const_iterator& pnameend, std::string::const_iterator& pvalueend);

// parse standalone
std::string::const_iterator parseXMLStandalone(std::string::const_iterator pc,  std::string& buffer, std::string::const_iterator& pnameend, std::string::const_iterator& pvalueend);

// parse xml end tag
std::string::const_iterator parseXMLEndTag(std::string::const_iterator pc, std::string& buffer, long& total, int& depth);

// parse xml start tag
std::string::const_iterator parseXMLStartTag(std::string::const_iterator pc, std::string& buffer, long& total, std::string& local_name, int& depth, bool& intag);

// parse xml namespace
std::string::const_iterator parseXMLNamespace(std::string::const_iterator pc, std::string& buffer, bool& intag);

// parse xml attribute
std::string::const_iterator parseXMLAttribute(std::string::const_iterator pc, std::string& buffer, bool& intag, std::string& local_name, std::string& value);

// parse xml CDATA
std::string::const_iterator parseXMLCDATA(std::string::const_iterator pc, std::string& buffer, std::string& characters, long& total);

// parse xml comment
std::string::const_iterator parseXMLComment(std::string::const_iterator pc, std::string& buffer, long total);

// parse characters before xml
std::string::const_iterator parseBeforeXML(std::string::const_iterator pc, std::string& buffer);

// parse xml entity reference
std::string::const_iterator parseXMLEntity(std::string::const_iterator pc, std::string& buffer, long total, std::string& characters);

// parse xml characters
std::string::const_iterator parseXMLCharacters(std::string::const_iterator pc, std::string& buffer, std::string& characters);

#endif
