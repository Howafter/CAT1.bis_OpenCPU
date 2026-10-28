#ifndef __L_UART_PORT_H__
#define __L_UART_PORT_H__


#include "lierda_api.h"

typedef struct
{
    char *name;
	uint8_t inti_state; // 0:未初始化，1：已初始化

	liot_uart_port_number_e uart_port;
	liot_uart_config_s uart;
	liot_uart_callback uart_cb;

} uart_port_config_t;

int l_uart_write(uart_port_config_t *config, void *data_ptr, int data_len) ;
int l_uart_read(uart_port_config_t *config, void *data_ptr, int data_len);
int l_uart_init(uart_port_config_t *config);
int l_uart_deinit(uart_port_config_t *config);

#endif // __L_UART_PORT_H__