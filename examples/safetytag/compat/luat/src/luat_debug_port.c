#include "luat_debug.h"

#include <stdarg.h>
#include <stdio.h>

#include "liot_os.h"
#include "liot_usb.h"

// ============================================================
// 模块调试开关变量定义
// 1 = 开启调试输出, 0 = 关闭调试输出
// 可根据需要在此修改默认值，或通过 AT 命令 / NVM 运行时修改
// ============================================================
uint8_t g_dbg_core      = 0;    // CORE 模块
uint8_t g_dbg_power     = 0;    // POWER 模块
uint8_t g_dbg_gnss      = 0;    // GNSS 模块
uint8_t g_dbg_mqtt      = 0;    // MQTT 模块
uint8_t g_dbg_net       = 0;    // NET 模块
uint8_t g_dbg_sensor    = 0;    // SENSOR 模块
uint8_t g_dbg_nrfdata   = 0;    // NRFDATA 模块
uint8_t g_dbg_adapter   = 0;    // ADAPTER 模块
uint8_t g_dbg_ntrip     = 0;    // NTRIP 模块
uint8_t g_dbg_audio     = 1;    // AUDIO 模块
uint8_t g_dbg_io        = 0;    // IO 模块 (botton, charge, led)
uint8_t g_dbg_dbus      = 0;    // DBUS 模块
uint8_t g_dbg_fota      = 0;    // FOTA 模块

/* Query the current state instead of consuming the platform's single USB
 * hotplug callback, which remains owned by charging and soft-off handling. */
static bool luat_debug_usb_connected(void)
{
    return liot_usb_get_hotplug_state() == LIOT_USB_HOTPLUG_IN;
}

void luat_debug_print(const char *fmt, ...)
{
    char log_buf[192];
    va_list args;

    if (!luat_debug_usb_connected()) {
        return;
    }

    va_start(args, fmt);
    vsnprintf(log_buf, sizeof(log_buf), fmt, args);
    va_end(args);

    LIOT_CUST_PRINTF("[luat] %s", log_buf);
}

void luat_debug_print_ex(const char *func, int line, const char *fmt, ...)
{
    char msg_buf[160];
    char log_buf[224];
    va_list args;

    if (!luat_debug_usb_connected()) {
        return;
    }

    va_start(args, fmt);
    vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
    va_end(args);

    snprintf(log_buf, sizeof(log_buf), "%s %d:[ luat ] %s", func ? func : "unknown", line, msg_buf);
    LIOT_CUST_PRINTF("%s", log_buf);
}
