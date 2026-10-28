/**
 * @File Name: customer_app.c
 * @brief  
 * @Author : zlc email:ciot_iot_support@lierda.com
 * @Version : 1.0
 * @Creat Date : 2023-09-08
 * 
 * @copyright Copyright (c) 2023 Lierda Science & Technology Group Co., Ltd.
 * 
 */
#include "lierda_api.h"

#if defined ( __GNUC__ )
/**
 * 对象字典回调信息 services.h 中的 INIT_OD_SERVICES 安置。
 * 由 gcc ld 连接器脚本 link.ld 设置 以下start和end地址。
 */
extern const uint32_t __od_jumptable_start;
extern const uint32_t __od_jumptable_end;
static const uint32_t od_jumptable_start = (uint32_t)&__od_jumptable_start;
static const uint32_t od_jumptable_end = (uint32_t)&__od_jumptable_end;

#elif defined ( __ICCARM__ )
static uint32_t od_jumptable_start;
static uint32_t od_jumptable_end;

#endif



/**
 * @description
 * 				1、下面的数据来自 连接器，参见 lierda_api.h 中的section定义 与 link.ld
 * 				2、初始化线程池
 */
void od_services_init(void* d, uint8_t nodeId)
{
	volatile uint32_t entry_address;
	volatile struct OD_services *entry;

	// od_threadpool = threadpool_create(THREADPOOL_OD_THREAD_COUNT, THREADPOOL_OD_THREAD_COUNT, "tpool_od_",
	// 		THREADPOOL_DEFAULT_STACK_SIZE, THREADPOOL_DEFAULT_PRIO);
	// if (od_threadpool == TX_NULL)
	// {
	// 	fatal_error_indicator(400, "E: %s\n", "OD->Init->TPC");
	// }

	/*
	 * 对所有具有回调函数设置的OD条目，统一安装
	 */
#if defined ( __ICCARM__ )
    #pragma section="od_services"
    od_jumptable_start = (uint32_t)__section_begin("od_services");
    od_jumptable_end = (uint32_t)__section_end("od_services");
#endif
	for (entry_address = od_jumptable_start; entry_address < od_jumptable_end; entry_address += sizeof(struct OD_services))
	{
		entry = (struct OD_services *)entry_address;

		if (entry->callback != NULL)
		{
			entry->callback(d);
			debug("OD entry callback executed, index: 0x%x", entry->combined_index);
		}
		else
		{
			debug("OD entry callback is NULL");
		}
		
	}

	return;
}

__attribute__((unused)) static int ODService_entry(void *params)
{
	// int r = 0;

	debug("ODService_entry");

	return 0;
}

INIT_OD_SERVICES(nt26fcnb60wna_demo) = {{.combined_index = COMBINE_INDEX(0x1000, 0x00), .callback = ODService_entry},};


/**
  \fn      void AppTask (void *argument)
  \brief   task app main entry function.
  \return
*/
void liot_nt26fcnb60wna_demo_thread (void *argument)
{
	
    while(1)
    {
		debug("liot_nt26fcnb60wna_demo_");
		liot_rtos_task_sleep_ms(5000);
    }
}

__WEAK uint32_t get_location_round_period_ms(void)	{return 0;}
__WEAK uint32_t get_g_s_gnss_timer1_interval(void)	{return get_location_round_period_ms();}
__WEAK uint32_t get_gnss_round_period_ms(void)	{return get_location_round_period_ms();}
__WEAK uint32_t get_dist_round_period_ms(void)	{return get_location_round_period_ms();}
__WEAK uint32_t get_bcon_round_period_ms(void)	{return get_location_round_period_ms();}
__WEAK uint32_t get_g_s_gnss_duration_down_interval(void)	{return 0;}
__WEAK uint32_t get_g_s_gnss_duration_run_interval(void)	{return 0;}
__WEAK uint32_t get_g_s_gnss_duration_switch_interval(void){return 0;}
__WEAK int data_ready_notify(void) {return 0;}
__WEAK uint8_t Trigger_GNSS(){return 0;}

__WEAK void _device_motion_notify(int ms) {}

// __WEAK int nrf_downlink_cmd_cache_read_and_clear(uint8_t *buffer, int bufsz) {return 0;}

__WEAK int usb_power_status_isopen(void) {return 0;}
__WEAK void reboot_device(void) {}
__WEAK void luat_vcom_resp(void *pdata, int length) {}

__WEAK void es8311_poweroff() {}


inline static uint32_t isr_vector_num_get(void)
{
    return __get_IPSR();
}

int rtos_queue_wait(liot_queue_t msgQRef, /* message queue reference */
                                           void *msg,       /* pointer to the message received */
                                           uint32 size,          /* size of the message */
                                           uint32 timeout        /* LIOT_WAIT_FOREVER, LIOT_NO_WAIT, or timeout */
)
{
	uint8 *recvMsg = (uint8 *)msg;

	return liot_rtos_queue_wait(msgQRef, recvMsg, size, timeout);
}

int rtos_queue_release(liot_queue_t msgQRef, /* message queue reference */
                                              uint32 size,          /* size of the message */
                                              void *msg,        /* start address of the data to be sent */
                                              uint32 timeout        /* LIOT_WAIT_FOREVER, LIOT_NO_WAIT, or timeout */
)
{
	uint8 *msgPtr = (uint8 *)msg;

	if(isr_vector_num_get() == 0)
	{
		return liot_rtos_queue_release(msgQRef, size, msgPtr, timeout);
	}
	else
	{
		return liot_rtos_queue_release_isr(msgQRef, size, msgPtr);
	}
}

void usb_drv_set(int status)
{
	if (status == 0)
    {
        liot_usb_drv_disable();
    }
    else
    {
        liot_usb_drv_enable();
    }
}


/******************************************************************************************************
 * Function: dev_set_modem_fun
 *
 * Description:
 *   设置设备 modem 功能
 *
 * Parameters:
 *	at_dst_fun		[in]		目标 modem 功能，范围：0,1,4 (liot_dev_cfun_e)
 *	rst				[in]		设置 modem 功能前是否重启 modem，取值：0-1
 *  nSim			[in]		SIM卡索引，取值：0-1
 * Return:
 *   LIOT_DATACALL_SUCCESS   成功
 *   other                   错误码
 *
 ******************************************************************************************************/
liot_errcode_dev_e dev_set_modem_fun(uint8_t at_dst_fun, uint8_t rst, uint8_t nSim)
{
	return liot_dev_set_modem_fun(at_dst_fun, rst, nSim);
}

liot_errcode_dev_e mobile_set_flymode(uint8_t nSim, uint8_t flymode)
{
	if (flymode == 0)
	{
		return dev_set_modem_fun(LIOT_DEV_CFUN_FULL, 0, nSim);
	}
	else
	{
		return dev_set_modem_fun(LIOT_DEV_CFUN_AIR, 0, nSim);
	}
	
}
