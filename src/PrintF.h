#pragma once
#include "sPrintF.h"

void next(const char*& s, TokenInfo& ans);

template <typename T>
class HasToString
{
private:
    typedef char YesType[1];
    typedef char NoType[2];

    template <typename C> static YesType& test( decltype(&C::c_str) ) ;
    template <typename C> static NoType& test(...);


public:
    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

template<typename ... Args>
static std::string fprintone(const char* format, Args ... a)
{
    int size_s = std::snprintf(nullptr, 0, format, a ...) + 1;
    if (size_s <= 0) {
        return "";
    }
    auto size = static_cast<size_t>(size_s);
    auto buffer = new char[size];
    std::snprintf(buffer, size, format, a ...);
    std::string ans(buffer, size - 1);
    delete[] buffer;
    return ans;
}

template<typename ... Args>
static void streamprint(std::stringstream& ss, const std::string& s, Args ... a) {
    ss << fprintone(s.c_str(), a ...);
}

template <TokenType T>
static auto parse(const char* arg) {
}

template <>
auto parse<TokenType::_int>(const char* arg) {
    char *endC;
    long i = strtol(arg, &endC, 10);
    if (!*endC)
    {
        return static_cast<int>(i);
    }
    else
    {
        if (strlen(arg) == 1)
            return static_cast<int>(arg[0]);
        return 0;
    }
}

template <>
auto parse<TokenType::_float>(const char* arg) {
    char* endC;
    double d = strtof(arg, &endC);
    if (!*endC) {
        return static_cast<float>(d);
    }
    else {
        return 0.0f;
    }
}

template <>
auto parse<TokenType::_char>(const char* arg) {
    const char def = ' ';
    if (strlen(arg) == 1) {
        return arg[0];
    }
    else {
        int ans = parse<TokenType::_int>(arg);
        return ans == 0 ? def : static_cast<char>(ans);
    }
}

template <>
auto parse<TokenType::string>(const char* arg) {
    return arg;
}

void next(const char*& s, TokenInfo& ans);

#define ihatecopying(__type) {                           \
    auto c = parse<TokenType::__type>((*arg++).c_str()); \
    switch (info.stars)                                  \
    {                                                    \
    case 1: streamprint(ss, info.ans, a, c); break;      \
    case 2: streamprint(ss, info.ans, b, a, c); break;   \
    default:streamprint(ss, info.ans, c); break;         \
    }                                                    \
    break; }                                             \

template<typename T> 
typename std::enable_if<HasToString<T>::value, std::string>::type Fprint(const char* cur, const std::vector<T>& args) {
    auto arg = args.begin();
    std::stringstream ss;
    TokenInfo info;
    while (*cur)
    {
        const char* l = cur;
        next(cur, info);
        switch (info.type)
        {
        case TokenType::skip: ss.write(l, cur - l); continue;  // info.ans is empty
        case TokenType::percent: ss << "%"; continue;
        default: break;
        }
        
        auto type = info.type;
        if (type != TokenType::string && type != TokenType::_char && type != TokenType::_float && type != TokenType::_int) {
            continue;
        } else if (arg == args.end()) {
            break;
        }

        int a = 0, b = 0;
        if (info.stars == 2) {
            b = parse<TokenType::_int>((*arg++).c_str());
            if (arg == args.end()) break;
            a = parse<TokenType::_int>((*arg++).c_str());
            if (arg == args.end()) break;
        }
        if (info.stars == 1) {
            a = parse<TokenType::_int>((*arg++).c_str());
            if (arg == args.end()) break;
        }

        switch (type)
        {
        case TokenType::_int: ihatecopying(_int)
        case TokenType::_float: ihatecopying(_float)
        case TokenType::_char: ihatecopying(_char)
        case TokenType::string: ihatecopying(string)
        }
    }
    return ss.str();
}

#undef ihatecopying
