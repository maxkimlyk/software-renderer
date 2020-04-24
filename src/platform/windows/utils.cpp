#include "../platform_dependent.h"

#include <windows.h>

uint32_t GetTimeMs() {
  return timeGetTime();
}
