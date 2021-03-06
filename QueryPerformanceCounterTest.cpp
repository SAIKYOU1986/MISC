/*
QueryPerformanceCounterTest.hpp
Author:  Adam Sawicki, http://asawicki.info, adam__REMOVE__@asawicki.info
Version: 1.0, 2017-12-02
License: Public Domain

This is a simple application that tests how long it takes to call
QueryPerformanceCounter WinAPI function. This function may be as fast as 10
nanoseconds or as slow as 2.5 microseconds, depending on platform.

Usage:
    QueryPerformanceCounterTest.exe [count]

count - numbers of times to call the function. Default: 1000000000.

See blog post:
http://asawicki.info/news_1667_when_queryperformancecounter_call_takes_long_time.html

*/
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdlib>
#include <cstdio>
#include <cstdint>

size_t count = 1000000000;

/// Division with mathematical rounding to nearest number.
template <typename T>
inline T round_div(T x, T y)
{
	return (x + (y / 2)) / y;
}

void PrintResult(const wchar_t* name, uint32_t milliseconds)
{
    double nsPerCall = (double)milliseconds * 1e6 / (double)count;
    uint32_t seconds = milliseconds / 1000; milliseconds %= 1000;
    uint32_t minutes = seconds / 60; seconds %= 60;
    uint32_t hours = minutes / 60; minutes %= 60;
    wprintf(L"According to %s it took %u:%02u:%02u.%03u (%g ns per call)\n", name, hours, minutes, seconds, milliseconds, nsPerCall);
}

int wmain(int argc, wchar_t **argv)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    if(argc == 2)
        swscanf_s(argv[1], L"%zu", &count);

    wprintf(L"Executing QueryPerformanceCounter x %zu...\n", count);

    LARGE_INTEGER begPerfCount;
    LARGE_INTEGER lastPerfCount;
    LARGE_INTEGER sumPerfCount{}; // Unused.

    uint64_t begTickCount = GetTickCount64();
    for(size_t i = 0; i < count; ++i)
    {
        QueryPerformanceCounter(&lastPerfCount);
        sumPerfCount.QuadPart += lastPerfCount.QuadPart;
        if(i == 0)
            begPerfCount.QuadPart = lastPerfCount.QuadPart;
    }
    uint64_t endTickCount = GetTickCount64();

    PrintResult(L"GetTickCount64", (uint32_t)(endTickCount - begTickCount));

    uint32_t qpcMilliseconds = (uint32_t)(uint64_t)round_div<int64_t>(
        (lastPerfCount.QuadPart - begPerfCount.QuadPart) * 1000,
        freq.QuadPart);
    PrintResult(L"QueryPerformanceCounter", qpcMilliseconds);

    return 0;
}
