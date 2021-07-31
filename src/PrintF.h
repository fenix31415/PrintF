#pragma once
#include <cstring>
#include "sPrintF.h"

extern char const DELIM = '`';

void next(const char*& s, TokenInfo& ans);
size_t fstrlen(const char*);

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
static char* fprintone(const char* format, Args ... a)
{
    int size_s = std::snprintf(nullptr, 0, format, a ...) + 1;
    if (size_s <= 0) {
        return nullptr;
    }
    auto size = static_cast<size_t>(size_s);
    auto buffer = new char[size];
    std::snprintf(buffer, size, format, a ...);
    return buffer;
}

template<typename ... Args>
static void streamprint(std::stringstream& ss, const std::string& s, Args ... a) {
    auto ans = fprintone(s.c_str(), a ...);
    if (ans) {
        ss << ans;
        delete[] ans;
    }
}

template <TokenType T>
static auto parse(const char* arg, bool defaultmode) { }

template <>
auto parse<TokenType::_int>(const char* arg, bool defaultmode) {
    if (defaultmode) return 0;
    char *endC;
    long i = strtol(arg, &endC, 10);
    if (!*endC || *endC == DELIM)
    {
        return static_cast<int>(i);
    }
    else
    {
        if (fstrlen(arg) == 1)
            return static_cast<int>(arg[0]);
        return 0;
    }
}

template <>
auto parse<TokenType::_float>(const char* arg, bool defaultmode) {
    if (defaultmode) return 0.0f;
    char* endC;
    double d = strtof(arg, &endC);
    if (!*endC || *endC == DELIM) {
        return static_cast<float>(d);
    }
    else {
        return 0.0f;
    }
}

template <>
auto parse<TokenType::_char>(const char* arg, bool defaultmode) {
    const char def = ' ';
    if (defaultmode) return def;
    if (arg[1] == '\0' || arg[1] == DELIM) {
        return arg[0];
    }
    else {
        int ans = parse<TokenType::_int>(arg, defaultmode);
        return ans == 0 ? def : static_cast<char>(ans);
    }
}


template<typename T, typename Enabled = void>
class params_t;

template<typename T>
class params_t<T, typename std::enable_if<HasToString<T>::value>::type> {
public:
	virtual const char* next() { return nullptr; }
	virtual bool end() const { return true; }
	virtual ~params_t() {};
	params_t() = default;
	params_t(const params_t& other) = delete;
	params_t& operator=(const params_t& other) = delete;
};

template<typename T, typename Enabled = void>
class params_array;

template<typename T>
class params_array<T, typename std::enable_if<HasToString<T>::value>::type> : public params_t<T, typename std::enable_if<HasToString<T>::value>::type> {
	typedef typename std::vector<T>::const_iterator it_type;
	it_type arg_iter;
	const it_type end_iter;
public:
	params_array(it_type arg_iter, const it_type end_iter) : arg_iter(arg_iter), end_iter(end_iter) {}
	virtual bool end() const {
		return arg_iter == end_iter;
	}
	virtual const char* next() {
		if (end()) return nullptr;

		return (*arg_iter++).c_str();
	}
};

template class params_array<std::string>;

template<typename T, typename Enabled = void>
class params_string;

template<typename T>
class params_string<T, typename std::enable_if<HasToString<T>::value>::type> : public params_t<T, typename std::enable_if<HasToString<T>::value>::type> {
	const char* cur_ptr;
public:
	params_string(const char* cur_ptr) : cur_ptr(cur_ptr) {}
	virtual bool end() const {
		return *cur_ptr == '\0';
	}
	virtual const char* next() {
		if (end()) return cur_ptr;

		auto ans = cur_ptr;
		while (*cur_ptr && *cur_ptr != DELIM) ++cur_ptr;
        if (*cur_ptr) ++cur_ptr;
		return ans;
	}
};
template class params_string<std::string>;


#define ihatecopying(__type) {                           \
    auto c = parse<TokenType::__type>(args.next(), defaultmode);      \
    switch (info.stars)                                  \
    {                                                    \
    case 1: streamprint(ss, info.ans, a, c); break;      \
    case 2: streamprint(ss, info.ans, b, a, c); break;   \
    default:streamprint(ss, info.ans, c); break;         \
    }                                                    \
    break; }  

template<typename T> 
typename std::enable_if<HasToString<T>::value, std::string>::type Fprint(const char* format, const T params) {
	params_string<T> params_storage(params.c_str());
	return Fprint(format, params_storage);
}

template<typename T> 
typename std::enable_if<HasToString<T>::value, std::string>::type Fprint(const char* format, const typename std::vector<T> params) {
    params_array<T> params_storage(params.begin(), params.end());
	return Fprint(format, params_storage);
}

template<typename T> 
typename std::enable_if<HasToString<T>::value, std::string>::type Fprint(const char* cur, params_t<T>& args) {
    std::stringstream ss;
    TokenInfo info;
    bool defaultmode = false;
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
        } else if (args.end()) {
            defaultmode = true;
            continue;
        }

        int a = 0, b = 0;
        if (info.stars == 2) {
            b = parse<TokenType::_int>(args.next(), defaultmode);
            if (args.end()) defaultmode = true;
            a = parse<TokenType::_int>(args.next(), defaultmode);
            if (args.end()) defaultmode = true;
        }
        if (info.stars == 1) {
            a = parse<TokenType::_int>(args.next(), defaultmode);
            if (args.end()) defaultmode = true;
        }

        switch (type)
        {
        case TokenType::_int: ihatecopying(_int)
        case TokenType::_float: ihatecopying(_float)
        case TokenType::_char: ihatecopying(_char)
        case TokenType::string: {
            if (defaultmode) {
                switch (info.stars)
                {
                case 1: streamprint(ss, info.ans, a, ""); break;
                case 2: streamprint(ss, info.ans, b, a, ""); break;
                default:streamprint(ss, info.ans, ""); break;
                }
            }
            else {
                auto c = args.next();
                if (args.end()) defaultmode = true;
                size_t len = fstrlen(c) + 1;
                auto new_c = (char*)malloc(len);
                if (!new_c) break;
                memcpy(new_c, c, len - 1);
                new_c[len - 1] = '\0';
                switch (info.stars)
                {
                case 1: streamprint(ss, info.ans, a, new_c); break;
                case 2: streamprint(ss, info.ans, b, a, new_c); break;
                default:streamprint(ss, info.ans, new_c); break;
                }
                free(new_c);
            }
            break;
        }
        default: break;
        }
    }
    return ss.str();
}

#undef ihatecopying
