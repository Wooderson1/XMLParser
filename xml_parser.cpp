/*
    xml_parser.cpp

    Implementation file for xml parsing
*/

#include "xml_parser.hpp"
#include "refillBuffer.hpp"
#include <iostream>
#include <cstring>

// check if declaration
bool isXMLDeclaration(std::string::const_iterator pc) {

    return (*pc == '<' && *std::next(pc) == '?');
}

// check if end tag
bool isXMLEndTag(std::string::const_iterator pc) {

    return (*pc == '<' && *std::next(pc) == '/');
}

// check if start tag
bool isXMLStartTag(std::string::const_iterator pc) {

    return (*pc == '<' && *std::next(pc) != '/' && *std::next(pc) != '?');
}

// check if namespace
bool isXMLNamespace(std::string::const_iterator& pc, std::string::const_iterator endpc,  bool intag) {

    const int XMLNS_SIZE = strlen("xmlns");

    return (intag && *pc != '>' && *pc != '/' && std::distance(pc, endpc) > (int) XMLNS_SIZE && std::string(pc, std::next(pc, XMLNS_SIZE)) == "xmlns"
    && (*std::next(pc, XMLNS_SIZE) == ':' || *std::next(pc, XMLNS_SIZE) == '='));
}

// check if attribute
bool isXMLAttribute(std::string::const_iterator pc, bool intag) {

    return (intag && *pc != '>' && *pc != '/');
}

// check if CDATA
bool isXMLCData(std::string::const_iterator pc) {

    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '[');
}

// check if comment
bool isXMLComment(std::string::const_iterator pc) {

    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '-' && *std::next(pc, 3) == '-');
}

// check if characters before XML
bool isBeforeXML(std::string::const_iterator pc, int depth) {

    return (*pc != '<' && depth == 0);
}

// check if entity
bool isXMLEntity(std::string::const_iterator pc) {

    return (*pc == '&');
}

// check if characters
bool isXMLCharacters(std::string::const_iterator pc) {

    return (*pc != '<');
}

// parse xml declaration
std::string::const_iterator parseXMLDeclaration(std::string::const_iterator pc, std::string& buffer, long& total) {

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

    return pc;
}

// parse required xml version
std::string::const_iterator parseXMLVersion(std::string::const_iterator pc,  std::string& buffer, std::string::const_iterator& pnameend, std::string::const_iterator& pvalueend) {

    auto endpc = std::find(pc, buffer.cend(), '>');
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

    return pc;
}

// parse encoding
std::string::const_iterator parseXMLEncoding(std::string::const_iterator pc,  std::string& buffer, std::string::const_iterator& pnameend, std::string::const_iterator& pvalueend) {

    auto endpc = std::find(pc, buffer.cend(), '>');
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

    return pc;
}

// parse standalone
std::string::const_iterator parseXMLStandalone(std::string::const_iterator pc,  std::string& buffer, std::string::const_iterator& pnameend, std::string::const_iterator& pvalueend) {

    auto endpc = std::find(pc, buffer.cend(), '>');
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

    return pc;
}

// parse xml end tag
std::string::const_iterator parseXMLEndTag(std::string::const_iterator pc, std::string& buffer, long& total, int& depth) {

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

    return pc;
}

// parse xml start tag
std::string::const_iterator parseXMLStartTag(std::string::const_iterator pc, std::string& buffer, long& total, std::string& local_name, int &depth, bool& intag) {

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
        std::advance(pc, 2);
        intag = false;
        --depth;
    }  

    return pc;
}

// parse xml namespace
std::string::const_iterator parseXMLNamespace(std::string::const_iterator pc, std::string& buffer, bool& intag) {

    const int XMLNS_SIZE = strlen("xmlns");
    std::advance(pc, XMLNS_SIZE);
    auto endpc = std::find(pc, buffer.cend(), '>');
    auto pnameend = std::find(pc, std::next(endpc), '=');

    if (pnameend == std::next(endpc)) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    pc = pnameend;
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

    return pc;
}

// parse xml attribute
std::string::const_iterator parseXMLAttribute(std::string::const_iterator pc, std::string& buffer, 
                                              bool& intag, std::string& local_name, std::string& value) {

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

    return pc;
}

// parse xml CDATA
std::string::const_iterator parseXMLCDATA(std::string::const_iterator pc, std::string& buffer, std::string& characters, long& total) {

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

    return pc;
}

// parse xml comment
std::string::const_iterator parseXMLComment(std::string::const_iterator pc, std::string& buffer, long total) {

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

    return pc;
}

// parse characters before xml
std::string::const_iterator parseBeforeXML(std::string::const_iterator pc, std::string& buffer) {

    pc = std::find_if_not(pc, buffer.cend(), [] (char c) { return isspace(c); });
    if (pc == buffer.cend() || !isspace(*pc)) {
        std::cerr << "parser error : Start tag expected, '<' not found\n";
        exit(1);
    }

    return pc;
}

// parse xml entity reference
std::string::const_iterator parseXMLEntity(std::string::const_iterator pc, std::string& buffer, long total, std::string& characters) {

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

    return pc;
}

// parse xml characters
std::string::const_iterator parseXMLCharacters(std::string::const_iterator pc, std::string& buffer, std::string& characters) {

    auto endpc = std::find_if(pc, buffer.cend(), [] (char c) { return c == '<' || c == '&'; });
    characters.assign(pc, endpc);
    pc = endpc;

    return pc;
}
