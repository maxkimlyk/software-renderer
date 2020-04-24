#ifndef _WINDOW_H_
#define _WINDOW_H_

#if defined(_WIN32) || defined(WIN32)
  #include "../platform/windows/window.h"
#elif defined(__unix__)
  #include "../platform/linux/window.h"
#endif

#endif
