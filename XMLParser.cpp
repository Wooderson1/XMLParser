/*
    XMLParser.cpp

    Implementation file for XML parsing class
*/

#include "XMLParser.hpp"
#include "refillBuffer.hpp"

#include <cstring>
#include <iostream>
#include <algorithm>

const int BUFFER_SIZE = 16 * 16 * 4096;
const int XMLNS_SIZE = strlen("xmlns");

// constructor
XMLParser::XMLParser(std::function<void(const std::string&, const std::string&, const std::string&)> handleDeclaration,
                     std::function<void(const std::string&, const std::string&)> handleStartTag,
                     std::function<void(const std::string&, const std::string&)> handleEndTag,
                     std::function<void(const std::string&, const std::string&)> handleAttribute,
                     std::function<void(const std::string&, const std::string&)> handleNamespace,
                     std::function<void(const std::string&, const long&)> handleCDATA,
                     std::function<void(const std::string&, const long&)> handleEntity,
                     std::function<void(const std::string&, const char&)> handleCharacters,
                     std::function<void()> handleComments)

    : handleDeclaration(handleDeclaration), handleStartTag(handleStartTag), handleEndTag(handleEndTag), handleAttribute(handleAttribute), 
      handleNamespace(handleNamespace), handleCDATA(handleCDATA), handleEntity(handleEntity), handleCharacters(handleCharacters), handleComments(handleComments)
{

    buffer.resize(BUFFER_SIZE, ' ');
    pc = buffer.cend();
}

// parse XML
void XMLParser::parse(long& total, std::string& characters, std::string& value, std::string& local_name) {

    while (true) {
        if (needRefill()) {

            // refill buffer
            refill(total);
            if (isDone()) {
                break;
            }

        } else if (isXMLDeclaration()) {

            // parse XML declaration
            parseXMLDeclaration(total);

        } else if (isXMLEndTag()) {
            // parse end tag
             parseXMLEndTag(total);

        } else if (isXMLStartTag()) {
            // parse start tag
            parseXMLStartTag(total, local_name);

        } else if (isXMLNamespace()) {

            // parse namespace
            parseXMLNamespace();

        } else if (isXMLAttribute()) {

            // parse attribute
            parseXMLAttribute(local_name, value);

        } else if (isXMLCData()) {

            // parse CDATA
            parseXMLCDATA(characters, total);

        } else if (isXMLComment()) {

            // parse XML comment
            parseXMLComment(total);

        } else if (isBeforeXML()) {

            // parse characters before or after XML
             parseBeforeXML();

        } else if (isXMLEntity()) {

            // parse entity references
            parseXMLEntity(total, characters);

        } else if (isXMLCharacters()) {

            // parse characters
            parseXMLCharacters(characters);
        }
    }
}

// does buffer need refilled
bool XMLParser::needRefill() {

    return (std::distance(pc, buffer.cend()) < 5);
}

// is done parsing
bool XMLParser::isDone() {

    return pc == buffer.cend();
}

// check if declaration
bool XMLParser::isXMLDeclaration() {

    return (*pc == '<' && *std::next(pc) == '?');
}

// check if end tag
bool XMLParser::isXMLEndTag() {

    return (*pc == '<' && *std::next(pc) == '/');
}

// check if start tag
bool XMLParser::isXMLStartTag() {

    return (*pc == '<' && *std::next(pc) != '/' && *std::next(pc) != '?');
}

// check if namespace
bool XMLParser::isXMLNamespace() {

    return (intag && *pc != '>' && *pc != '/' && std::distance(pc, buffer.cend()) > (int) XMLNS_SIZE && std::string(pc, std::next(pc, XMLNS_SIZE)) == "xmlns"
    && (*std::next(pc, XMLNS_SIZE) == ':' || *std::next(pc, XMLNS_SIZE) == '='));
}

// check if attribute
bool XMLParser::isXMLAttribute() {

    return (intag && *pc != '>' && *pc != '/');
}

// check if CDATA
bool XMLParser::isXMLCData() {

    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '[');
}

// check if comment
bool XMLParser::isXMLComment() {

    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '-' && *std::next(pc, 3) == '-');
}

// check if characters before XML
bool XMLParser::isBeforeXML() {

    return (*pc != '<' && depth == 0);
}

// check if entity
bool XMLParser::isXMLEntity() {

    return (*pc == '&');
}

// check if characters
bool XMLParser::isXMLCharacters() {

    return (*pc != '<');
}

// refill buffer
void XMLParser::refill(long& total) {

    pc = ::refillBuffer(pc, buffer, total);
}

// parse xml declaration
void XMLParser::parseXMLDeclaration(long& total) {

    auto endpc = std::find(pc, buffer.cend(), '>');
    if (endpc == buffer.cend()) {
        pc = refillBuffer(pc, buffer, total);
        endpc = std::find(pc, buffer.cend(), '>');
        if (endpc == buffer.cend()) {
            std::cerr << "parser error: Incomplete XML declaration\n";
            exit(1);
        }
    }
    std::advance(pc, strlen("<?xml"));
    pc = std::find_if_not(pc, endpc, [] (char c) { return isspace(c); });

    endpc = std::find(pc, buffer.cend(), '>');
    if (pc == endpc) {
        std::cerr << "parser error: Missing space after before version in XML declaration\n";
        exit(1);
    }
    pnameend = std::find(pc, endpc, '=');
    const std::string attr(pc, pnameend);
    pc = pnameend;
    std::advance(pc, 1);
    char delim = *pc;
    if (delim != '"' && delim != '\'') {
        std::cerr << "parser error: Invalid start delimiter for version in XML declaration\n";
        exit(1);
    }
    std::advance(pc, 1);
    pvalueend = std::find(pc, endpc, delim);
    if (pvalueend == endpc) {
        std::cerr << "parser error: Invalid end delimiter for version in XML declaration\n";
        exit(1);
    }
    if (attr != "version") {
        std::cerr << "parser error: Missing required first attribute version in XML declaration\n";
        exit(1);
    }
    const std::string version(pc, pvalueend);
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, endpc, [] (char c) { return isspace(c); });

    endpc = std::find(pc, buffer.cend(), '>');
    if (pc == endpc) {
        std::cerr << "parser error: Missing required encoding in XML declaration\n";
        exit(1);
    }
    pnameend = std::find(pc, endpc, '=');
    if (pnameend == endpc) {
        std::cerr << "parser error: Incomple encoding in XML declaration\n";
        exit(1);
    }
    const std::string attr2(pc, pnameend);
    pc = pnameend;
    std::advance(pc, 1);
    char delim2 = *pc;
    if (delim2 != '"' && delim2 != '\'') {
        std::cerr << "parser error: Invalid end delimiter for encoding in XML declaration\n";
        exit(1);
    }
    std::advance(pc, 1);
    pvalueend = std::find(pc, endpc, delim2);
    if (pvalueend == endpc) {
        std::cerr << "parser error: Incomple encoding in XML declaration\n";
        exit(1);
    }
    if (attr2 != "encoding") {
         std::cerr << "parser error: Missing required encoding in XML declaration\n";
         exit(1);
    }
    const std::string encoding(pc, pvalueend);
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, endpc, [] (char c) { return isspace(c); });

    endpc = std::find(pc, buffer.cend(), '>');
    if (pc == endpc) {
        std::cerr << "parser error: Missing required third attribute standalone in XML declaration\n";
        exit(1);
    }
    pnameend = std::find(pc, endpc, '=');
    const std::string attr3(pc, pnameend);
    pc = pnameend;
    std::advance(pc, 1);
    char delim3 = *pc;
    if (delim3 != '"' && delim3 != '\'') {
        std::cerr << "parser error : Missing attribute standalone delimiter in XML declaration\n";
        exit(1);
    }
    std::advance(pc, 1);
    pvalueend = std::find(pc, endpc, delim3);
    if (pvalueend == endpc) {
        std::cerr << "parser error : Missing attribute standalone in XML declaration\n";
        exit(1);
    }
    if (attr3 != "standalone") {
        std::cerr << "parser error : Missing attribute standalone in XML declaration\n";
        exit(1);
    }
    const std::string standalone(pc, pvalueend);
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, endpc, [] (char c) { return isspace(c); });
    std::advance(pc, strlen("?>"));
    pc = std::find_if_not(pc, buffer.cend(), [] (char c) { return isspace(c); });

    if (handleDeclaration != nullptr)
        handleDeclaration(version, encoding, standalone);
}

// parse xml end tag
void XMLParser::parseXMLEndTag(long& total) {

    --depth;
    auto endpc = std::find(pc, buffer.cend(), '>');
    if (endpc == buffer.cend()) {
        pc = refillBuffer(pc, buffer, total);
        endpc = std::find(pc, buffer.cend(), '>');
        if (endpc == buffer.cend()) {
            std::cerr << "parser error: Incomplete element end tag\n";
            exit(1);
        }
    }
    std::advance(pc, 2);
    auto pnameend = std::find_if(pc, std::next(endpc), [] (char c) { return isspace(c) || c == '>' || c == '/'; });
    if (pnameend == std::next(endpc)) {
          std::cerr << "parser error: Incomplete element end tag name\n";
          exit(1);
    }
    const std::string qname(pc, pnameend);
    const auto colonpos = qname.find(':');
    std::string prefixbase;
    if (colonpos != std::string::npos)
        prefixbase = qname.substr(0, colonpos);
    const std::string prefix = std::move(prefixbase);
    std::string local_namebase;
    if (colonpos != std::string::npos)
        local_namebase = qname.substr(colonpos + 1);
    else
        local_namebase = qname;
    const std::string local_name = std::move(local_namebase);
    pc = std::next(endpc);

    if (handleEndTag != nullptr)
        handleEndTag(local_name, prefix);
}

// parse xml start tag
void XMLParser::parseXMLStartTag(long& total, std::string& local_name) {

    auto endpc = std::find(pc, buffer.cend(), '>');
    if (endpc == buffer.cend()) {
        pc = refillBuffer(pc, buffer, total);
        endpc = std::find(pc, buffer.cend(), '>');
        if (endpc == buffer.cend()) {
            std::cerr << "parser error: Incomplete element start tag\n";
            exit(1);
        }
    }
    std::advance(pc, 1);
    auto pnameend = std::find_if(pc, std::next(endpc), [] (char c) { return isspace(c) || c == '>' || c == '/'; });
    if (pnameend == std::next(endpc)) {
        std::cerr << "parser error : Unterminated start tag '" << std::string(pc, pnameend) << "'\n";
        exit(1);
    }
    const std::string qname(pc, pnameend);
    const auto colonpos = qname.find(':');
    std::string prefixbase;
    if (colonpos != std::string::npos)
        prefixbase = qname.substr(0, colonpos);
    const std::string prefix = std::move(prefixbase);
    std::string local_namebase;
    if (colonpos != std::string::npos)
        local_namebase = qname.substr(colonpos + 1);
    else
        local_namebase = qname;
    local_name = std::move(local_namebase);
    pc = pnameend;
    pc = std::find_if_not(pc, std::next(endpc), [] (char c) { return isspace(c); });
    ++depth;
    intag = true;
    if (intag && *pc == '>') {
        std::advance(pc, 1);
        intag = false;
    }

    if (intag && *pc == '/' && *std::next(pc) == '>') {
        if (handleEndTag != nullptr)
            handleEndTag(local_name, prefix);
        std::advance(pc, 2);
        intag = false;
        --depth;
    }
    else {
        if (handleStartTag != nullptr)
            handleStartTag(local_name, prefix);
    }

}

// parse xml namespace
void XMLParser::parseXMLNamespace(){

    std::advance(pc, XMLNS_SIZE);
    auto endpc = std::find(pc, buffer.cend(), '>');
    auto pnameend = std::find(pc, std::next(endpc), '=');

    if (pnameend == std::next(endpc)) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
//    pc = pnameend;
    std::string prefix;
    if (*pc == ':') {
        std::advance(pc, 1);
        prefix.assign(pc, pnameend);
    }
    pc = std::next(pnameend);
    pc = std::find_if_not(pc, std::next(endpc), [] (char c) { return isspace(c); });
    if (pc == std::next(endpc)) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    const char delim = *pc;
    if (delim != '"' && delim != '\'') {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    std::advance(pc, 1);
    auto pvalueend = std::find(pc, std::next(endpc), delim);
    if (pvalueend == std::next(endpc)) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    const std::string uri(pc, pvalueend);
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, std::next(endpc), [] (char c) { return isspace(c); });
    if (intag && *pc == '>') {
        std::advance(pc, 1);
        intag = false;
    }
    if (intag && *pc == '/' && *std::next(pc) == '>') {
        std::advance(pc, 2);
        intag = false;
    }

    if (handleNamespace != nullptr)
        handleNamespace(uri, prefix);
}

// parse xml attribute
void XMLParser::parseXMLAttribute(std::string& local_name, std::string& value) {

    auto endpc = std::find(pc, buffer.cend(), '>');
    auto pnameend = std::find(pc, std::next(endpc), '=');
    if (pnameend == std::next(endpc))
        exit(1);
    const std::string qname(pc, pnameend);
    const auto colonpos = qname.find(':');
    std::string prefixbase;
    if (colonpos != std::string::npos)
        prefixbase = qname.substr(0, colonpos);
    const std::string prefix = std::move(prefixbase);
    std::string local_namebase;
    if (colonpos != std::string::npos)
        local_namebase = qname.substr(colonpos + 1);
    else
        local_namebase = qname;
    local_name = std::move(local_namebase);
    pc = std::next(pnameend);
    pc = std::find_if_not(pc, std::next(endpc), [] (char c) { return isspace(c); });
    if (pc == buffer.cend()) {
        std::cerr << "parser error : attribute " << qname << " incomplete attribute\n";
        exit(1);
    }
    char delim = *pc;
    if (delim != '"' && delim != '\'') {
        std::cerr << "parser error : attribute " << qname << " missing delimiter\n";
        exit(1);
    }
    std::advance(pc, 1);
    auto pvalueend = std::find(pc, std::next(endpc), delim);
    if (pvalueend == std::next(endpc)) {
        std::cerr << "parser error : attribute " << qname << " missing delimiter\n";
        exit(1);
    }

    value.assign(pc, pvalueend);

    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, std::next(endpc), [] (char c) { return isspace(c); });
    if (intag && *pc == '>') {
        std::advance(pc, 1);
        intag = false;
    }
    if (intag && *pc == '/' && *std::next(pc) == '>') {
        std::advance(pc, 2);
        intag = false;
    }

    if (handleAttribute != nullptr)
        handleAttribute(local_name, value);
}

// parse xml CDATA
void XMLParser::parseXMLCDATA(std::string& characters, long& total) {

    const std::string endcdata = "]]>";
    std::advance(pc, strlen("<![CDATA["));
    auto endpc = std::search(pc, buffer.cend(), endcdata.begin(), endcdata.end());
    if (endpc == buffer.cend()) {
        pc = refillBuffer(pc, buffer, total);
        endpc = std::search(pc, buffer.cend(), endcdata.begin(), endcdata.end());
        if (endpc == buffer.cend())
            exit(1);
    }
    characters.assign(pc, endpc);
    pc = std::next(endpc, strlen("]]>"));

    if (handleCDATA != nullptr) {
        handleCDATA(characters, total);
    }
}

// parse xml comment
void XMLParser::parseXMLComment(long total) {

    const std::string endcomment = "-->";
    auto endpc = std::search(pc, buffer.cend(), endcomment.begin(), endcomment.end());
    if (endpc == buffer.cend()) {
        pc = refillBuffer(pc, buffer, total);
        endpc = std::search(pc, buffer.cend(), endcomment.begin(), endcomment.end());
        if (endpc == buffer.cend()) {
            std::cerr << "parser error : Unterminated XML comment\n";
            exit(1);
        }
    }
    pc = std::next(endpc, strlen("-->"));
    pc = std::find_if_not(pc, buffer.cend(), [] (char c) { return isspace(c); });

    if (handleComments != nullptr) {
        handleComments();
    }
}

// parse characters before xml
void XMLParser::parseBeforeXML(){

    pc = std::find_if_not(pc, buffer.cend(), [] (char c) { return isspace(c); });
    if (pc == buffer.cend() || !isspace(*pc)) {
        std::cerr << "parser error : Start tag expected, '<' not found\n";
        exit(1);
    }
}

// parse xml entity reference
void XMLParser::parseXMLEntity(long total, std::string& characters) {

    if (std::distance(pc, buffer.cend()) < 3) {
        pc = refillBuffer(pc, buffer, total);
        if (std::distance(pc, buffer.cend()) < 3) {
            std::cerr << "parser error : Incomplete entity reference, '" << std::string(pc, buffer.cend()) << "'\n";
            exit(1);
        }
    }
    if (*std::next(pc) == 'l' && *std::next(pc, 2) == 't' && *std::next(pc, 3) == ';') {
        characters += '<';
        std::advance(pc, strlen("&lt;"));
    } else if (*std::next(pc) == 'g' && *std::next(pc, 2) == 't' && *std::next(pc, 3) == ';') {
        characters += '>';
        std::advance(pc, strlen("&gt;"));
    } else if (*std::next(pc) == 'a' && *std::next(pc, 2) == 'm' && *std::next(pc, 3) == 'p') {
        if (std::distance(pc, buffer.cend()) < 4) {
            pc = refillBuffer(pc, buffer, total);
            if (std::distance(pc, buffer.cend()) < 4) {
                std::cerr << "parser error : Incomplete entity reference, '" << std::string(pc, buffer.cend()) << "'\n";
                exit(1);
            }
        }
        if (*std::next(pc, 4) != ';') {
            const std::string partialEntity(pc, std::next(pc, 4));
            std::cerr << "parser error : Incomplete entity reference, '" << partialEntity << "'\n";
            exit(1);
        }
        characters += '&';
        std::advance(pc, strlen("&amp;"));
    } else {
        characters += '&';
        std::advance(pc, 1);
    }

    if (handleEntity != nullptr)
        handleEntity(characters, total);
}

// parse xml characters
void XMLParser::parseXMLCharacters(std::string& characters) {

    auto endpc = std::find_if(pc, buffer.cend(), [] (char c) { return c == '<' || c == '&'; });
    characters.assign(pc, endpc);
    pc = endpc;

    if (handleCharacters != nullptr)
        handleCharacters(characters, *pc);
}
