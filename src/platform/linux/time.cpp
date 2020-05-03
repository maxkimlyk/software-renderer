#include "../platform.h"

#include <time.h>

namespace sr
{

uint32_t GetTimeMs()
{
    static const clock_t CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
    return (uint32_t)(clock() / CLOCKS_PER_MS);
}

} // namespace sr
