#ifndef _UTILS_H_
#define _UTILS_H_

#define WARNING(...) fprintf (stderr, __VA_ARGS__)
#define ERROR(...) fprintf (stderr, __VA_ARGS__)
#define LOG(...) fprintf (stdout, __VA_ARGS__)
#define DELETE(ptr) if(ptr) delete (ptr)

#endif
