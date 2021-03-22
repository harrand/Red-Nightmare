#include "core/tz.hpp"
#include <cstdio>

int main()
{
    tz::initialise("Red Nightmare", tz::invisible_tag);
    std::printf("Hello, world!\n");
    tz::terminate();
}