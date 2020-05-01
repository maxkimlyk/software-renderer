#ifndef _PLATFORM_DEPENDENT_H_
#define _PLATFORM_DEPENDENT_H_

#if defined(_WIN32) || defined(WIN32)

#include "../platform/windows/window.h"
#include <windows.h>

#elif defined(__unix__)

#include "../platform/linux/window.h"
#include <string.h>

#endif

uint32_t GetTimeMs();

#endif
