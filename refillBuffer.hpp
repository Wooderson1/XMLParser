/*
    refillBuffer.hpp
*/

#ifndef REFILLBUFFER_HPP
#define REFILLBUFFER_HPP
#include <string>

// refill string buffer
std::string::const_iterator refillBuffer(std::string::const_iterator pc, std::string& buffer, long& totalBytes);

#endif
