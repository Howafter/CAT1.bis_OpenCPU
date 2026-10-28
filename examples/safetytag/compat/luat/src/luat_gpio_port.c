#include "luat_gpio.h"

#include <string.h>

#include "liot_gpio.h"

static liot_gpio_pull_mode_e luat_to_liot_pull(int pull)
{
    switch (pull)
    {
        case Luat_GPIO_PULLUP:
            return LIOT_FORCE_PULL_UP;
        case Luat_GPIO_PULLDOWN:
            return LIOT_FORCE_PULL_DOWN;
        default:
            return LIOT_PULL_DEFAULT;
    }
}

static liot_gpio_edge_mode_e luat_to_liot_edge(int irq_type)
{
    switch (irq_type)
    {
        case LUAT_GPIO_RISING_IRQ:
            return LIOT_EDGE_RISING;
        case LUAT_GPIO_FALLING_IRQ:
            return LIOT_EDGE_FALLING;
        case LUAT_GPIO_BOTH_IRQ:
            return LIOT_EDGE_BOTH;
        default:
            return LIOT_EDGE_RISING;
    }
}

void luat_gpio_set_default_cfg(luat_gpio_cfg_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    memset(cfg, 0, sizeof(*cfg));
    cfg->mode = LUAT_GPIO_INPUT;
    cfg->pull = Luat_GPIO_DEFAULT;
    cfg->irq_type = LUAT_GPIO_NO_IRQ;
    cfg->output_level = LUAT_GPIO_LOW;
}

int luat_gpio_open(luat_gpio_cfg_t *cfg)
{
    int pin;
    liot_gpio_pull_mode_e pull;

    if (cfg == NULL || cfg->pin < 0)
    {
        return -1;
    }
    pin = cfg->pin;
    pull = luat_to_liot_pull(cfg->pull);

    /* AON电源域：AON GPIO与普通GPIO电压域不同，需要单独使能 */
    if (cfg->is_aon)
    {
        liot_aon_power_on();
    }

     /* 引脚功能复用：pin_num非零时按alt_fun配置物理引脚的复用功能 */
    if (cfg->pin_num != 0)
    {
        liot_pin_set_func(cfg->pin_num, cfg->alt_fun);
    }

    if (cfg->mode == LUAT_GPIO_OUTPUT)
    {
        return (int)liot_gpio_init((liot_gpio_num_e)pin,
                                   LIOT_GPIO_OUTPUT,
                                   pull,
                                   cfg->output_level ? LIOT_LVL_HIGH : LIOT_LVL_LOW);
    }

    if (cfg->mode == LUAT_GPIO_IRQ)
    {
        int ret = (int)liot_gpio_init((liot_gpio_num_e)pin, LIOT_GPIO_INPUT, pull, LIOT_LVL_LOW);
        if (ret != 0)
        {
            return ret;
        }

        if (cfg->irq_type != LUAT_GPIO_NO_IRQ && cfg->irq_cb != NULL)
        {
            ret = (int)liot_int_register((liot_gpio_num_e)pin,
                                         LIOT_EDGE_TRIGGER,
                                         LIOT_DEBOUNCE_DIS,
                                         luat_to_liot_edge(cfg->irq_type),
                                         pull,
                                         (void *)cfg->irq_cb,
                                         cfg->irq_args);
            if (ret != 0)
            {
                return ret;
            }
            return (int)liot_int_enable((liot_gpio_num_e)pin);
        }

        return 0;
    }

    return (int)liot_gpio_init((liot_gpio_num_e)pin, LIOT_GPIO_INPUT, pull, LIOT_LVL_LOW);
}

int luat_gpio_close(int pin)
{
    if (pin < 0)
    {
        return -1;
    }

    (void)liot_int_disable((liot_gpio_num_e)pin);
    (void)liot_int_unregister((liot_gpio_num_e)pin);
    return (int)liot_gpio_deinit((liot_gpio_num_e)pin);
}

int luat_gpio_set(int pin, int level)
{
    if (pin < 0)
    {
        return -1;
    }

    return (int)liot_gpio_set_level((liot_gpio_num_e)pin, level ? LIOT_LVL_HIGH : LIOT_LVL_LOW);
}

int luat_gpio_get(int pin)
{
    liot_gpio_lvl_mode_e lvl = LIOT_LVL_LOW;

    if (pin < 0)
    {
        return -1;
    }

    if (liot_gpio_get_level((liot_gpio_num_e)pin, &lvl) != LIOT_GPIO_SUCCESS)
    {
        return -1;
    }

    return (int)lvl;
}

int luat_gpio_ctrl(int pin, int cmd, int param)
{
    if (pin < 0)
    {
        return -1;
    }

    if (cmd != LUAT_GPIO_CMD_SET_IRQ_MODE)
    {
        return -1;
    }

    if (param == LUAT_GPIO_NO_IRQ)
    {
        return (int)liot_int_disable((liot_gpio_num_e)pin);
    }

    return (int)liot_int_enable((liot_gpio_num_e)pin);
}
