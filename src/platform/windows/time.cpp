#include "../platform.h"

#include <windows.h>

namespace sr {

uint32_t GetTimeMs()
{
    return timeGetTime();
}

}
