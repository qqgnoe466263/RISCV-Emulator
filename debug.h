#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define CONFIG_DBG  1
#define ERROR       1

#define ERR(...) fprintf(stderr, __VA_ARGS__);

#ifdef CONFIG_DBG
#define DBG(...) fprintf(stdout, __VA_ARGS__);
#else
#define DBG(...);
#endif

#endif
