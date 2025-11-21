#pragma once

#include <iostream>
#include "Base/AllStatic.h"

#define REF(a) const a& 
#define CHECK_RETURN [[nodiscard]]

#define PROPERTY(type, name, start_init) \
protected:\
	type name = start_init; \
public:\
	type get_##name() {return name;}\
\

#define STR(x) #x
#define IncludeDown(a) STR(../a)

#ifdef interface
	#undef interface
#endif
#define interface struct
#define EMPTY_STR ""
#define NULLSTR EMPTY_STR

#ifndef USE_DEBUG
    #define Assert(expression) ((void)0)
#else
#define Assert(expression) \
        ((expression) ? (void)0 : []{ \
            std::cerr << "ASSERT: " << #expression << " @ " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::abort(); \
        }())
#endif

#define SingletonInstance(a) static a& Instance() {\
    \
        static a _; \
        return _; \
}

/* STRINGS MACROS */
#define Str2(str1, str2) (std::string(str1) + str2)
#define Str3(str1, str2, str3) Str2(str1, str2) + str3
#define Str4(str1, str2, str3, str4) Str3(str1, str2, str3) + str4
#define ToStr(obj) std::to_string(obj) // #include <string>


