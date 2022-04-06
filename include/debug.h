#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define ENOMEM  12      /* Out of memory */
#define EACCES  13      /* Permission denied */
#define EFAULT  14      /* Bad address */

#define CONFIG_DBG  0
#define ERROR       1

#define ERR(...) fprintf(stdout, __VA_ARGS__);

#if CONFIG_DBG
#define DBG(...) fprintf(stdout, __VA_ARGS__);
#else
#define DBG(...);
#endif

#endif
