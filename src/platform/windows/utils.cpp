#include "../platform.h"

#include <windows.h>

uint32_t GetTimeMs()
{
    return timeGetTime();
}
