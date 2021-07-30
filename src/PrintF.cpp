#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "sPrintF.h"

static bool is_flag(char c) {
    return c == '-' || c == '+' || c == ' ' || c == '0' || c == '\'' || c == '#';
}

static bool is_digit(char c) {
    return c <= '9' && c >= '0';
}


static bool _parse_char(const char*& i, char c) {
    if (*i != c) return false;
    ++i;
    return true;
}

static bool _parse_mbchar(const char*& i, char c) {
    _parse_char(i, c);
    return true;
}

static bool _parse_somechars_strong(const char*& i, bool (*f)(char)) {
    bool ans = false;
    while (f(*i)) {
        ans = true;
        ++i;
    }
    return ans;
}

static bool _parse_somechars(const char*& i, bool (*f)(char)) {
    _parse_somechars_strong(i, f);
    return true;
}

//static bool _parse_number(const char*& i) {
//    return _parse_somechars_strong(i, is_digit);
//}

static bool _parse_mbnumber(const char*& i) {
    return _parse_somechars(i, is_digit);
}


static bool parse_start(const char*& i) {
    return _parse_char(i, '%');
}

static bool parse_flags(const char*& i) {
    return _parse_somechars(i, is_flag);
}

// unsupported
//static bool parse_param(const char*& i) {
//    return !_parse_number(i) || _parse_char(i, '$');
//}

static bool parse_width(const char*& i, int& stars) {
    if (_parse_char(i, '*')) {
        ++stars;
        return true;
    }
    return _parse_mbnumber(i);
}

static bool parse_prec(const char*& i, int& stars) {
    if (!_parse_char(i, '.')) return true;
    if (_parse_char(i, '*')) {
        ++stars;
        return true;
    }
    return _parse_mbnumber(i);
}

static bool parse_length(const char*& i) {
    return _parse_char(i, 'L') || _parse_char(i, 'z') || _parse_char(i, 'j') || _parse_char(i, 't') ||
        (_parse_char(i, 'h') && _parse_mbchar(i, 'h')) || (_parse_char(i, 'l') && _parse_mbchar(i, 'l')) || true;
}

static bool parse_type(const char*& i) {
    return _parse_char(i, '%') || _parse_char(i, 'd') || _parse_char(i, 'i') || _parse_char(i, 'u') || _parse_char(i, 'f')
        || _parse_char(i, 'F') || _parse_char(i, 'e') || _parse_char(i, 'E') || _parse_char(i, 'g') || _parse_char(i, 'G')
        || _parse_char(i, 'x') || _parse_char(i, 'X') || _parse_char(i, 'o') || _parse_char(i, 's') || _parse_char(i, 'c')
        || _parse_char(i, 'p') || _parse_char(i, 'a') || _parse_char(i, 'A') || _parse_char(i, 'n');
}

void next(const char*& s, TokenInfo& ans) {
    if (*s != '%') {
        while (*s && *s != '%') ++s;
        ans = { "", 0, TokenType::skip };
        return;
    }
    auto l = s;
    int stars = 0;
    bool b = parse_start(s) && /*parse_param(s) &&*/ parse_flags(s) && parse_width(s, stars) && parse_prec(s, stars) && parse_length(s);
    auto m = s;
    b = b && parse_type(s);
    if (b) {
        std::string res(l, s - l);
        switch (*m)
        {
        case '%': ans = { res, stars, TokenType::percent }; return;
        case 'd':
        case 'u':
        case 'x':
        case 'X':
        case 'o':
        case 'i': ans = { res, stars, TokenType::_int }; return;
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
        case 'f':
        case 'F': ans = { res, stars, TokenType::_float }; return;
        case 'c': ans = { res, stars, TokenType::_char }; return;
        case 'p':
        case 's': ans = { res, stars, TokenType::string }; return;
        default:  break;
        }
        
    }
    ans = { "", 0, TokenType::err };
}

