#include "luat_init_export.h"
#include "luat_rtos.h"
#include "liot_log.h"

/* Non-hardware test registration; replace with real driver only after pin review. */
static void safetytag_driver_probe(void)
{
    printf("safetytag: INIT_DRV_EXPORT reached\n");
    luat_rtos_task_sleep(1);
}
INIT_DRV_EXPORT(safetytag_driver_probe, 10);

static void safetytag_task_probe(void)
{
    printf("safetytag: INIT_TASK_EXPORT reached (business disabled)\n");
}
INIT_TASK_EXPORT(safetytag_task_probe, 10);
