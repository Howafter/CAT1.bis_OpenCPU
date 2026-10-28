/*
* 通过INIT_HW_EXPORT INIT_DRV_EXPORT INIT_TASK_EXPORT这3个宏修饰后，系统初始化的时候会调用function，不同的宏涉及到调用顺序不同
* 大优先级上`INIT_HW_EXPORT` > `INIT_DRV_EXPORT` > `INIT_TASK_EXPORT`
* 这3个宏有统一用法`INIT_HW_EXPORT(function, level)`，其中function就是入口函数，level是小优先级，填0,1,2...，数字越小，优先级越高。

* `INIT_HW_EXPORT`一般用于硬件初始化，GPIO之类的，可以没有
* `INIT_DRV_EXPORT`一般用于外设驱动初始化，初始化外部器件之类的，打开电源，提供时钟之类的，可以没有
* `INIT_TASK_EXPORT`一般用于初始化任务，用户代码基本上都是跑在任务里，原则上必须有
*/

#ifndef LUAT_INIT_EXPORT_H
#define LUAT_INIT_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*luat_init_task_func_t)(void);

typedef struct
{
    int level;
    luat_init_task_func_t func;
} luat_init_task_entry_t;

typedef luat_init_task_entry_t luat_init_hw_entry_t;
typedef luat_init_task_entry_t luat_init_drv_entry_t;

/* 使用与链接脚本一致的段名（不带前导点） */
#define INIT_HW_EXPORT(func, level) \
    static const luat_init_hw_entry_t __luat_init_hw_##func __attribute__((used, section("luat_init_hw"))) = {level, func}

#define INIT_DRV_EXPORT(func, level) \
    static const luat_init_drv_entry_t __luat_init_drv_##func __attribute__((used, section("luat_init_drv"))) = {level, func}

#define INIT_TASK_EXPORT(func, level) \
    static const luat_init_task_entry_t __luat_init_task_##func __attribute__((used, section("luat_init_task"))) = {level, func}

void luat_run_init_tasks(void);

#ifdef __cplusplus
}
#endif

#endif