#include "luat_init_export.h"

void user_main(void)
{
    /* Entry runs in the SDK appUserTask, not before the RTOS starts. */
    luat_run_init_tasks();
}
