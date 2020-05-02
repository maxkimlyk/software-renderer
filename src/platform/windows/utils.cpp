#include "../platform.h"

#include <windows.h>

namespace {

uint32_t GetTimeMs()
{
    return timeGetTime();
}

}
