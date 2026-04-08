#pragma once


using S8 = signed char;
using S16 = signed short;
using S32 = signed int;
#ifdef _WIN32
	using S64 = signed long long;
#else
	using S64 = signed long;
#endif

using Size = unsigned long long;

using U8 = unsigned char;
using U16 = unsigned short;
using U32 = unsigned int;
#ifdef _WIN32
	using U64 = unsigned long long;
#else
	using U64 = unsigned long;
#endif


static_assert(sizeof(S8) == 1, "S8 is wrong");
static_assert(sizeof(S16) == 2, "S16 is wrong");
static_assert(sizeof(S32) == 4, "S32 is wrong");
static_assert(sizeof(S64) == 8, "S64 is wrong");

static_assert(sizeof(Size) == 8, "Size is wrong");

static_assert(sizeof(U8) == 1, "U8 is wrong");
static_assert(sizeof(U16) == 2, "U16 is wrong");
static_assert(sizeof(U32) == 4, "U32 is wrong");
static_assert(sizeof(U64) == 8, "U64 is wrong");
