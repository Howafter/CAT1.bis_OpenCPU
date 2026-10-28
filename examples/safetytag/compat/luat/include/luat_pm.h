#ifndef LUAT_PM_H
#define LUAT_PM_H


#include <stdint.h>
#include "liot_power.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LUAT_PM_SLEEP_MODE_NONE     LIOT_SLP_ACTIVE_STATE	 //< 系统处于活跃状态，未采取任何的降低功耗状态
#define LUAT_PM_SLEEP_MODE_IDLE     LIOT_SLP_IDLE_STATE	 //< 空闲模式，该模式在系统空闲时停止 CPU 和部分时钟，任意事件或中断均可以唤醒
#define LUAT_PM_SLEEP_MODE_LIGHT    LIOT_SLP_SLP1_STATE	 //< 轻度睡眠模式，CPU 停止，多数时钟和外设停止
#define LUAT_PM_SLEEP_MODE_DEEP     LIOT_SLP_SLP2_STATE	 //< 深度睡眠模式，CPU 停止，仅少数低功耗外设工作，可被特殊中断唤醒
#define LUAT_PM_SLEEP_MODE_STANDBY  LIOT_SLP_HIB_STATE //< 待机模式，CPU 停止，设备上下文丢失(可保存至特殊外设)，唤醒后通常复位
#define LUAT_PM_SLEEP_MODE_SHUTDOWN	5	//<关断模式，比 Standby 模式功耗更低， 上下文通常不可恢复， 唤醒后复位


#define LUAT_PM_WAKEUP_FROM_POR     LIOT_WAKEUP_FROM_POR
#define LUAT_PM_WAKEUP_FROM_RTC     LIOT_WAKEUP_FROM_RTC
#define LUAT_PM_WAKEUP_FROM_PAD     LIOT_WAKEUP_FROM_PAD
#define LUAT_PM_WAKEUP_FROM_LPUART  LIOT_WAKEUP_FROM_LPUART
#define LUAT_PM_WAKEUP_FROM_LPUSB   LIOT_WAKEUP_FROM_LPUSB
#define LUAT_PM_WAKEUP_FROM_PWRKEY  LIOT_WAKEUP_FROM_PWRKEY
#define LUAT_PM_WAKEUP_FROM_CHARG   LIOT_WAKEUP_FROM_CHARG
#define LUAT_PM_WAKEUP_FROM_T3412   LIOT_WAKEUP_FROM_T3412


int luat_pm_set_sleep_mode(int mode, const char *tag);
int luat_pm_get_wakeup_reason(void);
int luat_pm_reboot(void);
int luat_pm_set_usb_power(int enabled);
int luat_pm_get_vbus_status(uint8_t *status);

#ifdef __cplusplus
}
#endif

#endif