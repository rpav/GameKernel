#pragma once

#include <sstream>
#include <iostream>

inline void convstr(std::stringstream&) { }

template<typename First, typename...Rest>
inline void convstr(std::stringstream &ss, First arg, Rest...args) {
    ss << arg;
    convstr(ss, args...);
}

template<typename First, typename...Rest>
inline std::string str(First arg, Rest...args) {
    std::stringstream ss;
    convstr(ss, arg, args...);
    return ss.str();
}

#if 1
#include <iostream>
#define say log_print

template<typename...Rest>
inline void log_print(Rest...args) {
    auto s = str(args...);
#ifdef ANDROID
    __android_log_print(ANDROID_LOG_DEBUG, ANDROID_APPNAME, "%s", s.c_str());
#else
    std::cerr << s << std::endl;
#endif
}
#else
#define LOG(...)
#endif // NDEBUG
