#ifndef LUAT_DEBUG_H
#define LUAT_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lierda_log.h"

void luat_debug_print(const char *fmt, ...);
void luat_debug_print_ex(const char *func, int line, const char *fmt, ...);

#define LUAT_DEBUG_PRINT(format, ...) luat_debug_print_ex(__func__, __LINE__, format, ##__VA_ARGS__)

// ============================================================
// 模块调试开关 - 运行时 debug 控制
// 每模块独立 uint8_t 变量，非零时输出该模块日志
// ============================================================
extern uint8_t g_dbg_core;      // CORE 模块
extern uint8_t g_dbg_power;    // POWER 模块
extern uint8_t g_dbg_gnss;      // GNSS 模块
extern uint8_t g_dbg_mqtt;      // MQTT 模块
extern uint8_t g_dbg_net;       // NET 模块
extern uint8_t g_dbg_sensor;    // SENSOR 模块
extern uint8_t g_dbg_nrfdata;   // NRFDATA 模块
extern uint8_t g_dbg_adapter;   // ADAPTER 模块
extern uint8_t g_dbg_ntrip;     // NTRIP 模块
extern uint8_t g_dbg_audio;     // AUDIO 模块
extern uint8_t g_dbg_io;        // IO 模块 (botton, charge, led)
extern uint8_t g_dbg_dbus;      // DBUS 模块
extern uint8_t g_dbg_fota;      // FOTA 模块

/**
 * @brief 模块级调试宏：仅当对应 debug 开关非零时输出日志
 * @param dbg_var  模块的调试开关变量 (如 g_dbg_gnss)
 * @param fmt      格式化字符串
 * @param ...      可变参数
 */
#define DBG_MODULE(dbg_var, fmt, ...)      \
    do {                                     \
        if ((dbg_var)) {                     \
            luat_debug_print_ex(__func__, __LINE__, fmt, ##__VA_ARGS__); \
        }                                    \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif