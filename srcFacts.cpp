/*
    srcFacts.cpp

    Produces a report with various counts of the number of 
    statements, declarations, etc. of a source code project
    in C++, C, Java, and C#.

    Input is an XML file in the srcML format.

    Code includes an almost-complete XML parser. Limitations:
    * DTD declarations are not handled
    * Well-formedness is not checked
*/

#include "XMLParser.hpp"
#include <iostream>
#include <algorithm>

int main() {

    std::string url;
    int textsize = 0;
    int loc = 0;
    int expr_count = 0;
    int function_count = 0;
    int class_count = 0;
    int file_count = 0;
    int decl_count = 0;
    int comment_count = 0;
    int return_count = 0;
    int string_count = 0;
    int line_comment_count = 0;
    long total = 0;
    std::string local_name;
    std::string value;
    std::string characters;

     XMLParser parser(

        // handleDeclaration(), unneeded
        nullptr,

        // count srcML items from Start Tag
        [&expr_count, &function_count, &decl_count, &class_count,
         &file_count, &comment_count, &return_count](const std::string& local_name, const std::string& prefix) {

            if (local_name == "expr")
                ++expr_count;
            else if (local_name == "function")
                ++function_count;
            else if (local_name == "decl")
                ++decl_count;
            else if (local_name == "class")
                ++class_count;
            else if (local_name == "unit")
                ++file_count;
            else if (local_name == "comment")
                ++comment_count;
            else if (local_name == "return")
                ++return_count;
        },

        // XML End Tag, unneeded
        nullptr,

        // update srcML url and count items from attributte
        [&url, &string_count, &line_comment_count]
        (const std::string& local_name, const std::string& value) {

            if (local_name == "url")
                url = value;
            if (value == "string")
                ++string_count;
            if (value == "line")
                ++line_comment_count;
        },

        // handleNamespace(), unneeded
        nullptr,

        // update textsize and loc from CDATA
        [&textsize, &loc](const std::string& characters, const long& total) {

            textsize += (int) characters.size();
            loc += (int) std::count(characters.begin(), characters.end(), '\n');
        },

        // update textsize from entity
        [&textsize](const std::string& characters, const long& total) {

            textsize += (int) characters.size();
        },

        // update srcML items from characters
        [&textsize, &loc](const std::string& characters, const char& c) {

            loc += (int) std::count(characters.cbegin(), characters.cend(), '\n');
            textsize += (int) characters.size();
        },

        // XML comment count, unnneeded
        nullptr
    );

    // parse XML
    parser.parse(total, characters, value, local_name);

    // srcML report
    std::cout << "# srcFacts: " << url << '\n';
    std::cout << "| Item | Count |\n";
    std::cout << "|:-----|-----:|\n";
    std::cout << "| srcML | " << total << " |\n";
    std::cout << "| files | " << file_count << " |\n";
    std::cout << "| LOC | " << loc << " |\n";
    std::cout << "| characters | " << textsize << " |\n";
    std::cout << "| classes | " << class_count << " |\n";
    std::cout << "| functions | " << function_count << " |\n";
    std::cout << "| declarations | " << decl_count << " |\n";
    std::cout << "| expressions | " << expr_count << " |\n";
    std::cout << "| comments | " << comment_count << " |\n";
    std::cout << "| returns | " << return_count << " |\n";
    std::cout << "| string literals | " << string_count << " |\n";
    std::cout << "| line comments | " << line_comment_count << " |\n";

    return 0;
}
