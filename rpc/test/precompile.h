#ifndef PRECOMPILE_H
#define PRECOMPILE_H

#if defined(_MSC_VER) && (_MSC_VER < 1600)
// No stdint.h on VC 2008.
typedef char					int8_t;
typedef unsigned char			uint8_t;
typedef short					int16_t;
typedef unsigned short			uint16_t;
typedef int						int32_t;
typedef unsigned int			uint32_t;
typedef __int64					int64_t;
typedef unsigned __int64		uint64_t;
#else
#include <stdint.h>
#endif

#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <new>

#define ZLIB_CONST
#include "zlib/zlib.h"

#endif
