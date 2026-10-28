#ifndef __LIERDA_API_H__
#define __LIERDA_API_H__


#include <stdio.h>
#include <string.h>
#include "stdlib.h"

#include "liot_gpio.h"
#include "liot_os.h"
#include "lierda_log.h"
#include "lierda_app_main.h"

#include "liot_datacall.h"
#include "liot_adc.h"
#include "liot_uart.h"
#include "liot_nw.h"
#include "liot_dev.h"
#include "liot_spi.h"
#include "liot_rtc.h"
#include "liot_fs_api.h"
#include "liot_mqtt_client.h"
#include "liot_osi_def.h"
#include "liot_usb.h"
#include "liot_hal_audio.h"
#include "liot_http.h"

#include "mbedtls/base64.h"
// #include "liot_data_call.h"
#include "sockets.h"
#include "luat_init_export.h"
#include "luat_debug.h"


// #define UNUSED __attribute__((unused))

/**
 * 通过 gcc ld 在连接阶段组合 ".od_services." 段。
 * 需注意 link.ld 脚本
 */
#if defined ( __GNUC__ )
#define INIT_OD_SERVICES(name)	\
	__USED const struct OD_services OD_services_##name[] __attribute__((section(".od_services." #name)))
#elif defined ( __ICCARM__ )
#define INIT_OD_SERVICES(name)	\
    __root const struct OD_services OD_services_##name[] @ "od_services"
#endif

/**
 * 将OD的主下标与子下表组合成一个uint32_t长度的组合下标。
 * 用于服务查找。
 */
#define COMBINE_INDEX(master, sub) \
	(((master) << 16) | sub)
/* 从组合下标中提取主下标 */
#define MASTER_INDEX(combine_idx) \
	(combine_idx >> 16)
/* 从组合下标中提取子下标 */
#define SUB_INDEX(combine_idx) \
	(combine_idx & 0xff)


typedef int (*callback)(void *);

/**
 * OD 回调函数表
 */
struct OD_services
{
	/*
	 * 主索引与子索引的联合索引，用于服务查找。
	 * combined_index = index << 16 | sub_index;
	 */
	uint32_t	combined_index;

	/* 回调 */
	callback 	callback;
};

void od_services_init(void* d, uint8_t Id);


/* os start */
#define RTOS_WAIT_FOREVER	portMAX_DELAY

//不可在中断中调用该函数
#define rtos_ENTER_CRITICAL()					vPortEnterCritical()
#define rtos_EXIT_CRITICAL()					vPortExitCritical()

#define rtos_task_sleep_ms(ms)  liot_rtos_task_sleep_ms(ms)
#define rtos_task_sleep_s(s)  	liot_rtos_task_sleep_s(s)

#define rtos_task_create(taskRef, stackSize, priority, taskName, taskStart, argv, ...)  liot_rtos_task_create(taskRef, stackSize, priority, taskName, taskStart, argv, ##__VA_ARGS__);
#define rtos_task_delete(taskRef)  liot_rtos_task_delete(taskRef)

#define rtos_task_suspend(taskRef)  liot_rtos_task_suspend(taskRef)
#define rtos_task_resume(taskRef)  liot_rtos_task_resume(taskRef)

#define rtos_timer_create(timerRef, cyclicalEn, callBackRoutine, timerArgc)  liot_rtos_timer_create(timerRef, cyclicalEn, callBackRoutine, timerArgc)
#define rtos_timer_start(timerRef, setTime)  liot_rtos_timer_start(timerRef, setTime)
#define rtos_timer_stop(timerRef)  liot_rtos_timer_stop(timerRef)
#define rtos_timer_delete(timerRef)  liot_rtos_timer_delete(timerRef)
#define rtos_timer_is_running(timerRef)  liot_rtos_timer_is_running(timerRef)


//设置最大计数值
#define rtos_semaphore_create_ex(semaRef, initialCount, max_cnt)	liot_rtos_semaphore_create_ex(semaRef, initialCount, max_cnt)
#define rtos_semaphore_wait(semaRef, timeout)						liot_rtos_semaphore_wait(semaRef, timeout)
#define rtos_semaphore_release(semaRef)								liot_rtos_semaphore_release(semaRef)
#define rtos_semaphore_get_cnt(semaRef, cntPtr)						liot_rtos_semaphore_get_cnt(semaRef, cntPtr)
#define rtos_semaphore_delete(semaRef)								liot_rtos_semaphore_delete(semaRef)

#define rtos_mutex_create(mutexRef)  liot_rtos_mutex_create(mutexRef)
#define rtos_mutex_lock(mutexRef, timeout)  liot_rtos_mutex_lock(mutexRef, timeout)
#define rtos_mutex_try_lock(mutexRef)  liot_rtos_mutex_try_lock(mutexRef)
#define rtos_mutex_unlock(mutexRef)  liot_rtos_mutex_unlock(mutexRef)
#define rtos_mutex_delete(mutexRef)  liot_rtos_mutex_delete(mutexRef)

#define rtos_flag_create(flagRef)  liot_rtos_flag_create(flagRef)
#define rtos_flag_get(flagRef)  liot_rtos_flag_get(flagRef)
#define rtos_flag_wait(flagRef, mask, operation, flag, timeout)  liot_rtos_flag_wait(flagRef, mask, operation, flag, timeout)
#define rtos_flag_release(flagRef, mask, operation)  liot_rtos_flag_release(flagRef, mask, operation)
#define rtos_flag_clear(flagRef, mask)  liot_rtos_flag_clear(flagRef, mask)

#define rtos_queue_create(msgQRef, maxSize, maxNumber)  liot_rtos_queue_create(msgQRef, maxSize, maxNumber)
// #define rtos_queue_wait(msgQRef, recvMsg, size, timeout)  liot_rtos_queue_wait(msgQRef, recvMsg, size, timeout)
// #define rtos_queue_release_isr(msgQRef, size, msgPtr)  liot_rtos_queue_release_isr(msgQRef, size, msgPtr)
// #define rtos_queue_release(msgQRef, size, msgPtr, timeout)  liot_rtos_queue_release(msgQRef, size, msgPtr, timeout)
int rtos_queue_wait(liot_queue_t msgQRef, /* message queue reference */
                                           void *msg,       /* pointer to the message received */
                                           uint32 size,          /* size of the message */
                                           uint32 timeout        /* LIOT_WAIT_FOREVER, LIOT_NO_WAIT, or timeout */
);

int rtos_queue_release(liot_queue_t msgQRef, /* message queue reference */
                                              uint32 size,          /* size of the message */
                                              void *msg,        /* start address of the data to be sent */
                                              uint32 timeout        /* LIOT_WAIT_FOREVER, LIOT_NO_WAIT, or timeout */
);
#define rtos_queue_get_cnt(msgQRef, cnt)  liot_rtos_queue_get_cnt(msgQRef, cnt)
#define rtos_queue_delete(msgQRef)  liot_rtos_queue_delete(msgQRef)

#define rtos_get_system_tick() liot_rtos_get_system_tick()
/* os end */


/* io start */
enum
{
	IO_FUNC_0 = 0,
	IO_FUNC_1,
	IO_FUNC_2,
	IO_FUNC_3,
	IO_FUNC_4,
	IO_FUNC_5,
	IO_FUNC_6,
	IO_FUNC_7,
};

#define gpio_set_level(gpio_num, gpio_lvl) liot_gpio_set_level(gpio_num, gpio_lvl)
#define gpio_get_level(gpio_num, gpio_lvl) liot_gpio_get_level(gpio_num, gpio_lvl)
#define int_register(gpio_num, gpio_trigger, gpio_debounce, gpio_edge, gpio_pull, int_cb, cb_ctx) \
	liot_int_register(gpio_num, gpio_trigger, gpio_debounce, gpio_edge, gpio_pull, int_cb, cb_ctx)
#define int_enable(gpio_num) liot_int_enable(gpio_num)
#define int_disable(gpio_num) liot_int_disable(gpio_num)

#define adc_get_volt_raw(channel_id, div, adc_value) liot_adc_get_volt_raw(channel_id, div, adc_value)

// #define uart_write(port, data, data_len) liot_uart_write(port, data, data_len)
// #define uart_read(port, data, data_len) liot_uart_read(port, data, data_len)

#define spi_write_read(port, inbuf, outbuf, len) liot_spi_write_read(port, inbuf, outbuf, len)
#define spi_read(port, buf, len) liot_spi_read(port, buf, len)
#define spi_write(port, buf, len) liot_spi_write(port, buf, len)
#define spi_release(port) liot_spi_release(port)

#define dev_get_imei(p_imei, imei_len, nSim) liot_dev_get_imei(p_imei, imei_len, nSim)
#define mobile_get_signal_strength(nSim, pt_info) liot_nw_get_signal_strength(nSim, pt_info)

liot_errcode_dev_e mobile_set_flymode(uint8_t nSim, uint8_t flymode);

void usb_drv_set(int status);
/* io end */

/* rtc start */
#define rtc_set_time(tm) liot_rtc_set_time(tm)
#define rtc_get_time(tm) liot_rtc_get_time(tm)
/* rtc end */

/* sim start */
// #define sim_get_iccid(iccid, len, nSim) liot_sim_get_iccid(iccid, len, nSim)
/* sim end */

// #define debug(format, ...) LIOT_CUST_PRINTF(format, ##__VA_ARGS__)
// #define debug_v(format,  ...) LIOT_PRINTF(UNILOG_LIOT_CUST, P_VALUE, format, ##__VA_ARGS__)

// NRFData 模块日志受 g_dbg_nrfdata 开关控制
#undef LUAT_DEBUG_PRINT
#define LUAT_DEBUG_PRINT(fmt, ...) DBG_MODULE(g_dbg_nrfdata, fmt, ##__VA_ARGS__)
#define debug(fmt, ...) LUAT_DEBUG_PRINT(fmt, ##__VA_ARGS__)
// #define base64_encode(dst, dlen, olen, src, slen) liot_base64_encode(dst, dlen, olen, src, slen)


#define BUILD_EVENT_MASK(bit)       (uint32_t)(0x01 << bit)

#define DEBUG_I_FSKV_ENABLE
#define DEBUG_I_GPIO_ENABLE
#define DEBUG_I_MQTT_ENABLE
#define DEBUG_I_SPI_ENABLE
#define DEBUG_I_UART_ENABLE

#define DEBUG_APP_BUZZER_ENABLE
#define DEBUG_APP_ZW111_ENABLE
#define DEBUG_APP_KEY_ENABLE
#define DEBUG_APP_LED_ENABLE
#define DEBUG_APP_MOTOR_ENABLE
#define DEBUG_APP_SERIAL_IRQ_ENABLE
#define DEBUG_APP_SERIAL_SWITCH_ENABLE
#define DEBUG_APP_STATE_MACHINE_ENABLE
#define DEBUG_APP_RTK_NMEA_ENABLE

#endif
