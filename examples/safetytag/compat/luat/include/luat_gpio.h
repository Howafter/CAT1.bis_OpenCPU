#ifndef LUAT_GPIO_H
#define LUAT_GPIO_H

#include <stdint.h>
#include "liot_gpio2.h"
#include "liot_type.h"
#ifdef __cplusplus
extern "C" {
#endif

// /* Define HAL GPIO enum for backward compatibility and easy migration */
// typedef enum {
//     HAL_GPIO_0 = 0,
//     HAL_GPIO_1,
//     HAL_GPIO_2,
//     HAL_GPIO_3,
//     HAL_GPIO_4,
//     HAL_GPIO_5,
//     HAL_GPIO_6,
//     HAL_GPIO_7,
//     HAL_GPIO_8,
//     HAL_GPIO_9,
//     HAL_GPIO_10,
//     HAL_GPIO_11,
//     HAL_GPIO_12,
//     HAL_GPIO_13,
//     HAL_GPIO_14,
//     HAL_GPIO_15,
//     HAL_GPIO_16,
//     HAL_GPIO_17,
//     HAL_GPIO_18,
//     HAL_GPIO_19,
//     HAL_GPIO_20,
//     HAL_GPIO_21,
//     HAL_GPIO_22,
//     HAL_GPIO_23,
//     HAL_GPIO_24,
//     HAL_GPIO_25,
//     HAL_GPIO_26,
//     HAL_GPIO_27,
//     HAL_GPIO_28,
//     HAL_GPIO_29,
//     HAL_GPIO_30,
//     HAL_GPIO_31,
//     HAL_GPIO_MAX,
//     HAL_GPIO_NONE = HAL_GPIO_MAX,
//     HAL_WAKEUP_0 = HAL_GPIO_MAX,    // EC618 special input IOs, WAKEUPPAD0~2 and PWRKEY
//     HAL_WAKEUP_1,
//     HAL_WAKEUP_2,
//     HAL_WAKEUP_PWRKEY,
//     HAL_GPIO_QTY,
// } hal_gpio_num_e;

enum {
    LUAT_GPIO_INPUT = 0,
    LUAT_GPIO_OUTPUT = 1,
    LUAT_GPIO_IRQ = 2,
};

#define LUAT_GPIO_LOW 0
#define LUAT_GPIO_HIGH 1

enum {
    Luat_GPIO_DEFAULT = 0,
    Luat_GPIO_PULLUP = 1,
    Luat_GPIO_PULLDOWN = 2,
};

#define LUAT_GPIO_DEFAULT Luat_GPIO_DEFAULT
#define LUAT_GPIO_PULLUP Luat_GPIO_PULLUP
#define LUAT_GPIO_PULLDOWN Luat_GPIO_PULLDOWN

enum {
    LUAT_GPIO_NO_IRQ = 0,
    LUAT_GPIO_RISING_IRQ = 1,
    LUAT_GPIO_FALLING_IRQ = 2,
    LUAT_GPIO_BOTH_IRQ = 3,
    LUAT_GPIO_HIGH_IRQ = 4,
    LUAT_GPIO_LOW_IRQ = 5,
};

enum {
    LUAT_GPIO_CMD_SET_IRQ_MODE = 0,
};

typedef void (*luat_gpio_irq_cb)(void *data);

typedef struct luat_gpio_cfg
{
    int pin; /**<引脚，对应 liot_gpio_num_e GPIO编号*/
    uint8_t mode;/**<GPIO模式*/
    uint8_t pull;/**<GPIO上下拉模式*/
    uint8_t irq_type;/**<GPIO中断模式*/
    uint8_t output_level;/**<GPIO输出高低电平选择*/
    luat_gpio_irq_cb irq_cb;/**<GPIO中断回调函数*/
    void* irq_args;/**<GPIO中断回调时用户参数*/
    uint8_t alt_fun;/**<引脚功能复用选择值，传给 liot_pin_set_func 的 func_sel 参数*/
    uint8_t pin_num;/**<模组物理引脚编号，非零时调用 liot_pin_set_func 配置引脚复用；0 表示不配置复用*/
    uint8_t is_aon;/**<是否为 AON 电源域引脚：1=使能 AON 电源域(liot_aon_power_on)，0=普通 GPIO 域*/
} luat_gpio_cfg_t;

void luat_gpio_set_default_cfg(luat_gpio_cfg_t *cfg);
int luat_gpio_open(luat_gpio_cfg_t *cfg);
int luat_gpio_close(int pin);
int luat_gpio_set(int pin, int level);
int luat_gpio_get(int pin);
int luat_gpio_ctrl(int pin, int cmd, int param);

#ifdef __cplusplus
}
#endif

#endif
