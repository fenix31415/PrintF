#pragma once
#include <string>

enum class TokenType {
    err, percent, skip, _int, _float, _char, string
};

struct TokenInfo {
    std::string ans = "";
    int stars = 0;
    TokenType type = TokenType::err;
};

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    if (size_s <= 0) {
        return "";
    }
    auto size = static_cast<size_t>(size_s);
    auto buffer = new char[size];
    std::snprintf(buffer, size, format.c_str(), args ...);
    std::string ans(buffer, size - 1);
    delete[] buffer;
    return ans;
}
