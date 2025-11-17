#include <iostream>
#include <cstdarg>
#include <cstdio>

using namespace std;

void Log(int level, const char *format, ...)
{
    // 方法1：使用 va_list 和 vprintf
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n"); // 换行
}

int main()
{
    Log(0, "1233123""%s,%s", "hello", "world");
    return 0;
}