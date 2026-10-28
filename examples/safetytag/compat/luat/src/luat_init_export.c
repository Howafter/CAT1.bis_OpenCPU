#include "luat_init_export.h"

#include <stddef.h>

/* GCC 链接器会为各个 init 段生成 __start/__stop 符号（由链接脚本定义）。 */
extern const luat_init_hw_entry_t __start_luat_init_hw[];
extern const luat_init_hw_entry_t __stop_luat_init_hw[];

extern const luat_init_drv_entry_t __start_luat_init_drv[];
extern const luat_init_drv_entry_t __stop_luat_init_drv[];

extern const luat_init_task_entry_t __start_luat_init_task[];
extern const luat_init_task_entry_t __stop_luat_init_task[];

static void luat_run_init_section(const luat_init_task_entry_t *start, const luat_init_task_entry_t *stop)
{
    const luat_init_task_entry_t *entry = NULL;
    int max_level = -1;

    for (entry = start; entry < stop; ++entry)
    {
        if ((entry->func != NULL) && (entry->level > max_level))
        {
            max_level = entry->level;
        }
    }

    if (max_level < 0)
    {
        return;
    }

    for (int current_level = 0; current_level <= max_level; ++current_level)
    {
        for (entry = start; entry < stop; ++entry)
        {
            if ((entry->func == NULL) || (entry->level != current_level))
            {
                continue;
            }
            entry->func();
        }
    }
}

void luat_run_init_tasks(void)
{
    /* 大优先级顺序：HW > DRV > TASK */
    luat_run_init_section(__start_luat_init_hw, __stop_luat_init_hw);
    luat_run_init_section(__start_luat_init_drv, __stop_luat_init_drv);
    luat_run_init_section(__start_luat_init_task, __stop_luat_init_task);
}