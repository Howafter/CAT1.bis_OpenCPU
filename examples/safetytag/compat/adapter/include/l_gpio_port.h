#ifndef __L_GPIO_PORT_H__
#define __L_GPIO_PORT_H__

#include "lierda_api.h"

typedef struct
{
    char *name;
	uint8_t inti_state; // 0:未初始化，1：已初始化
    uint8_t en_state;   // 0:低电平有效，1：高电平有效
    uint8_t agpio;      // 是否为AGPIO口，AGPIO口需要单独使能电源域

    uint8_t pin_num;
    uint8_t func_sel;

    liot_gpio_num_e gpio_num;
    liot_gpio_dir_e gpio_dir;
    liot_gpio_pull_mode_e gpio_pull;
    liot_gpio_lvl_mode_e gpio_lvl;

    /* interrupt config */
    liot_gpio_trigger_mode_e gpio_trigger;
    liot_gpio_debounce_mode_e gpio_debounce;
    liot_gpio_edge_mode_e gpio_edge;
    void *int_cb;
    void *cb_ctx;

}io_config_t;

int l_gpio_disable(io_config_t *cfg);
int l_gpio_enable(io_config_t *cfg);
int l_gpio_get_level(io_config_t *cfg);
void l_gpio_init(io_config_t *cfg);
void l_gpio_deinit(io_config_t *cfg);

void l_irq_init(io_config_t *cfg);
void l_irq_enable(io_config_t *cfg);
void l_irq_disable(io_config_t *cfg);
void l_irq_deinit(io_config_t *cfg);

#endif // __L_GPIO_PORT_H__