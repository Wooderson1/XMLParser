/*
    xmlstats.cpp

    Markdown report with the number of each part of XML.
    E.g., the number of start tags, end tags, attributes,
    character sections, etc.
*/

#include "XMLParser.hpp"
#include <iostream>

int main() {

    long total = 0;
    int decl_count = 0;
    int start_tag_count = 0;
    int end_tag_count = 0;
    int character_count = 0;
    int attribute_count = 0;
    int namespace_count = 0;
    int comment_count = 0;
    int CDATA_count = 0;
    std::string local_name;
    std::string value;
    std::string characters;

    XMLParser parser(

        // count xml declerations
        [&decl_count](const std::string& value, const std::string& encoding, const std::string& standalon) {

            ++decl_count;
        },

        // count Start Tag
        [&start_tag_count](const std::string& local_name, const std::string& prefix) {

            ++start_tag_count;
        },

        // count End Tag
        [&end_tag_count](const std::string& local_name, const std::string& prefix) {

            ++end_tag_count;
        },

        // ount Attribute
        [&attribute_count](const std::string& local_name, const std::string& value) {

            ++attribute_count;
        },

        // count namespaces
        [&namespace_count](const std::string& uri, const std::string& a) {
            ++namespace_count;
        },

        // count CDATA
        [&CDATA_count](const std::string& local_name, const long& total) {

            ++CDATA_count;
        },

        // Entity, unneeded
        nullptr,

        // count characters
        [&character_count](const std::string& characters, const char& c) {

            ++character_count;
        },

        // count commments
        [&comment_count]() {
            ++comment_count;
        }
    );

    // parse XML
    parser.parse(total, characters, value, local_name);

    // XML Report
    std::cout << "| Item | Count |\n";
    std::cout << "|:-----|------:|\n";
    std::cout << "| XML declerations | " << decl_count << " |\n";
    std::cout << "| start tags | " << start_tag_count << " |\n";
    std::cout << "| end tags | " << end_tag_count << " |\n";
    std::cout << "| character sections | " << character_count << " |\n";
    std::cout << "| attributes | " << attribute_count << " |\n";
    std::cout << "| namespaces | " << namespace_count << " |\n";
    std::cout << "| comments | " << comment_count << " |\n";
    std::cout << "| CDATA | " << CDATA_count << " |\n";

    return 0;
}
