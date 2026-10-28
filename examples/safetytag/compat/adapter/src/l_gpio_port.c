#include "l_gpio_port.h"

/*********************************** gpio start ***********************************/
int l_gpio_disable(io_config_t *cfg)
{
	if (cfg->inti_state == 1)
	{
		liot_gpio_set_level(cfg->gpio_num, !cfg->en_state);
	}
	else
	{
		debug("->%s not initialized", cfg->name);
	}

	return 0;
}

int l_gpio_enable(io_config_t *cfg)
{
    if (cfg->inti_state == 1)
    {
        liot_gpio_set_level(cfg->gpio_num, cfg->en_state);
    }
    else
    {
        debug("->%s not initialized", cfg->name);
    }

    return 0;
}

int l_gpio_get_level(io_config_t *cfg)
{
	liot_gpio_lvl_mode_e gpio_level = 0;

	if (cfg->inti_state == 1)
	{
		liot_gpio_get_level(cfg->gpio_num, &gpio_level);
	}
	else
	{
		debug("->%s not initialized", cfg->name);
	}

	return gpio_level;
}

void l_gpio_init(io_config_t *cfg)
{
	if (cfg->inti_state == 0)
	{
		if (cfg->agpio == 1)
		{
			//使能AON POWER AON与普通GPIO电压不同，需要单独使能电源域
			liot_aon_power_on();
		}
		
		// 切换IO电压到3.3V
		liot_gpio_set_voltage(LIOT_VOL_3_30V);

		liot_pin_set_func(cfg->pin_num, cfg->func_sel);
		liot_gpio_init(cfg->gpio_num, cfg->gpio_dir, cfg->gpio_pull, cfg->gpio_lvl);

		cfg->inti_state = 1;

		liot_gpio_set_level(cfg->gpio_num, !cfg->en_state);

		debug("io->%s init successful", cfg->name);
	}
}

void l_gpio_deinit(io_config_t *cfg)
{
	if (cfg->inti_state == 1)
	{
		liot_gpio_deinit(cfg->gpio_num);
		cfg->inti_state = 0;

		// if (cfg->agpio == 1)
		// {
		// 	//使能AON POWER AON与普通GPIO电压不同，需要单独使能电源域
		// 	liot_aon_power_off();
		// }

		debug("io->%s deinit successful", cfg->name);
	}
}
/*********************************** gpio end ***********************************/


/*********************************** interrupt start ***********************************/
void l_irq_init(io_config_t *cfg)
{
	if (cfg->inti_state == 0)
	{
		if (cfg->agpio == 1)
		{
			//使能AON POWER AON与普通GPIO电压不同，需要单独使能电源域
			liot_aon_power_on();
		}
		
		// 切换IO电压到3.3V
		// liot_gpio_set_voltage(LIOT_VOL_3_30V);

		//设置测试引脚的复用功能为GPIO功能，引脚的复用功能请查看对应型号的IO复用表
		liot_pin_set_func(cfg->pin_num, cfg->func_sel);
		
		/*
		* 初始化MAIN_RI 引脚为输入模式，默认上拉
		* 初始化MAIN_RI 引脚为中断默认，并注册回调函数，并将info作为参数传入回调函数 
		* 使能MAIN_RI 引脚中断
		*/
		liot_gpio_init(cfg->gpio_num, cfg->gpio_dir, cfg->gpio_pull, cfg->gpio_lvl);
		liot_int_register(cfg->gpio_num, cfg->gpio_trigger, cfg->gpio_debounce, cfg->gpio_edge, cfg->gpio_pull, cfg->int_cb, cfg->cb_ctx);
		liot_int_enable(cfg->gpio_num);	

		cfg->inti_state = 1;

		debug("irq->%s init successful", cfg->name);
	}
}

void l_irq_enable(io_config_t *cfg)
{
	if (cfg->inti_state == 1)
	{
		liot_int_enable(cfg->gpio_num);
	}
}

void l_irq_disable(io_config_t *cfg)
{
	if (cfg->inti_state == 1)
	{
		liot_int_disable(cfg->gpio_num);	
	}
}

void l_irq_deinit(io_config_t *cfg)
{
	if (cfg->inti_state == 1)
	{
		liot_int_disable(cfg->gpio_num);
		liot_gpio_deinit(cfg->gpio_num);

		cfg->inti_state = 0;

		debug("irq->%s deinit successful", cfg->name);
	}
}
/*********************************** interrupt end ***********************************/


/*********************************** adc start ***********************************/

/*********************************** adc end ***********************************/