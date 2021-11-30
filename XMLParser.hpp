/*
    XMLParser.hpp

    Decleration file for XML parsing class
*/

#ifndef INCLUDED_XMLPARSER_HPP
#define INCLUDED_XMLPARSER_HPP

#include <string>
#include <functional>

class XMLParser {
public:

    // constructor
    XMLParser(std::function<void(const std::string&, const std::string&, const std::string&)> handleDeclaration,
              std::function<void(const std::string&, const std::string&)> handleStartTag,
              std::function<void(const std::string&, const std::string&)> handleEndTag,
              std::function<void(const std::string&, const std::string&)> handleAttribute,
              std::function<void(const std::string&, const std::string&)> handleNamespace,
              std::function<void(const std::string&, const long&)> handleCDATA,
              std::function<void(const std::string&, const long&)> handleEntity,
              std::function<void(const std::string&, const char&)> handleCharacters,
              std::function<void()> handleComments);

    // parse XML
    void parse(long& total, std::string& characters, std::string& value, std::string& local_name);

    // does buffer need refilled
    bool needRefill();

    // is done parsing
    bool isDone();

    // check if declaration
    bool isXMLDeclaration();

    // check if end tag
    bool isXMLEndTag();

    // check if start tag
    bool isXMLStartTag();

    // check if namespace
    bool isXMLNamespace();

    // check if attribute
    bool isXMLAttribute();

    // check if CDATA
    bool isXMLCData();

    // check if comment
    bool isXMLComment();

    // check if characters before XML
    bool isBeforeXML();

    // check if entity
    bool isXMLEntity();

    // check if characters
    bool isXMLCharacters();

    // refill buffer 
    void refill(long& total);

    // parse xml declaration
    void parseXMLDeclaration(long& total);

    // parse xml end tag
    void parseXMLEndTag(long& total);

    // parse xml start tag
    void parseXMLStartTag(long& total, std::string& local_name);

    // parse xml namespace
    void parseXMLNamespace();

    // parse xml attribute
    void parseXMLAttribute(std::string& local_name, std::string& value);

    // parse xml CDATA
    void parseXMLCDATA(std::string& characters, long& total);

    // parse xml comment
    void parseXMLComment(long total);

    // parse characters before xml
    void parseBeforeXML();

    // parse xml entity reference
    void parseXMLEntity(long total, std::string& characters);

    // parse xml characters
    void parseXMLCharacters(std::string& characters);

private:
    std::function<void(const std::string&, const std::string&, const std::string&)> handleDeclaration;
    std::function<void(const std::string&, const std::string&)> handleStartTag;
    std::function<void(const std::string&, const std::string&)> handleEndTag;
    std::function<void(const std::string&, const std::string&)> handleAttribute;
    std::function<void(const std::string&, const std::string&)> handleNamespace;
    std::function<void(const std::string&, const long&)> handleCDATA;
    std::function<void(const std::string&, const long&)> handleEntity;
    std::function<void(const std::string&, const char&)> handleCharacters;
    std::function<void()> handleComments;

    bool intag = false;
    std::string buffer;
    std::string::const_iterator pc;
    std::string::const_iterator pnameend;
    std::string::const_iterator pvalueend;
    int depth = 0;
};

#endif
