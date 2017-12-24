#pragma once

#define WINDOWS 1

#ifdef WINDOWS
//windows平台下会使用到的常量

#define INTEGER int
#define UINTEGER unsigned int
#define INT8 char
#define INT16 short
#define INT32 long
#define UINT8 unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned long
#define FLOAT float
#define UFLOAT unsigned float
#define DOUBLE double
#define UDOUBLE unsigned double

#endif // WINDOWS

#ifdef LINUX
#define SOCKET int
#endif