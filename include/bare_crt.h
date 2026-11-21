#ifndef NOETIC_BARE_CRT_H
#define NOETIC_BARE_CRT_H

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#if defined(_WIN32) && defined(NOETIC_BARE_WIN64)

typedef struct _iobuf FILE;
extern FILE* __cdecl __acrt_iob_func(unsigned index);
#define stdin  (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))

extern int    __cdecl printf(const char* fmt, ...);
extern int    __cdecl fprintf(FILE* stream, const char* fmt, ...);
extern int    __cdecl snprintf(char* buf, size_t n, const char* fmt, ...);
extern int    __cdecl sscanf(const char* str, const char* fmt, ...);
extern int    __cdecl fflush(FILE* stream);
extern FILE*  __cdecl fopen(const char* filename, const char* mode);
extern char*  __cdecl fgets(char* str, int num, FILE* stream);
extern int    __cdecl fclose(FILE* stream);

extern void*  __cdecl malloc(size_t size);
extern void*  __cdecl calloc(size_t num, size_t size);
extern void   __cdecl free(void* ptr);
extern void   __cdecl qsort(void* base, size_t nmemb, size_t size, int (__cdecl *compar)(const void*, const void*));
extern int    __cdecl atoi(const char* str);
extern unsigned long long __cdecl strtoull(const char* str, char** endptr, int base);

extern void*  __cdecl memcpy(void* dst, const void* src, size_t n);
extern void*  __cdecl memset(void* dst, int c, size_t n);
extern int    __cdecl strcmp(const char* a, const char* b);
extern int    __cdecl strncmp(const char* a, const char* b, size_t n);

extern double __cdecl log(double x);
extern double __cdecl log2(double x);
extern double __cdecl exp(double x);
extern double __cdecl pow(double x, double y);
extern double __cdecl sqrt(double x);
extern double __cdecl fabs(double x);
extern double __cdecl sin(double x);
extern double __cdecl cos(double x);
extern double __cdecl tanh(double x);
extern double __cdecl atan2(double y, double x);
extern double __cdecl hypot(double x, double y);
extern double __cdecl fmod(double x, double y);
extern double __cdecl erfc(double x);
extern long long __cdecl llround(double x);

static inline long long noetic_llabs(long long v) { return v < 0 ? -v : v; }
#define llabs noetic_llabs
static inline int noetic_isfinite_f(float x) { return (x == x) && (x - x == 0.0f); }
#define isfinite(x) noetic_isfinite_f((float)(x))

typedef int BOOL;
__declspec(dllimport) BOOL __stdcall QueryPerformanceCounter(int64_t* lpPerformanceCount);
__declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(int64_t* lpFrequency);

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#endif

#endif /* NOETIC_BARE_CRT_H */
