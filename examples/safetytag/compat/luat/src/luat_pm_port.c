#include "luat_pm.h"

#include "liot_power.h"
#include "liot_usb.h"
int luat_pm_set_sleep_mode(int mode, const char *tag)
{
    liot_sleep_depth_e sleep_depth;

    (void)tag;

    switch (mode)
    {
        case LUAT_PM_SLEEP_MODE_NONE:
            sleep_depth = LIOT_SLP_ACTIVE_STATE;
            break;
        case LUAT_PM_SLEEP_MODE_IDLE:
            sleep_depth = LIOT_SLP_IDLE_STATE;
            break;
        case LUAT_PM_SLEEP_MODE_LIGHT:
            sleep_depth = LIOT_SLP_SLP1_STATE;
            break;
        case LUAT_PM_SLEEP_MODE_DEEP:
            sleep_depth = LIOT_SLP_SLP2_STATE;
            break;
        case LUAT_PM_SLEEP_MODE_STANDBY:
        case LUAT_PM_SLEEP_MODE_SHUTDOWN:
            sleep_depth = LIOT_SLP_HIB_STATE;
            break;
        default:
            return -1;
    }

    return liot_set_sleep_depth(sleep_depth) == LIOT_SLEEP_SUCCESS ? 0 : -1;
}

int luat_pm_get_wakeup_reason(void)
{
    return (int)liot_get_wakeup_src();
}

int luat_pm_reboot(void)
{
    return liot_power_reset(LIOT_RESET_NORMAL) == LIOT_POWER_RESET_SUCCESS ? 0 : -1;
}

int luat_pm_set_usb_power(int enabled)
{
    (void)enabled;
    /*USB电源开关移动到task_charge.c中实现,插入USB打开,拔出关闭*/
    return 0;
}

int luat_pm_get_vbus_status(uint8_t *status)
{
    if (status == NULL)
    {
        return -1;
    }
    *status = liot_usb_get_hotplug_state() == LIOT_USB_HOTPLUG_IN ? 1 : 0;
    return 0;
}
