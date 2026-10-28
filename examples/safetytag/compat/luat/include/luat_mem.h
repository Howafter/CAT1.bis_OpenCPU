#ifndef LUAT_MEM_H
#define LUAT_MEM_H

#include <stddef.h>

#include "liot_os.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void *luat_heap_malloc(size_t size)
{
    return liot_rtos_malloc(size);
}

static inline void luat_heap_free(void *ptr)
{
    liot_rtos_free(ptr);
}

#ifdef __cplusplus
}
#endif

#endif
