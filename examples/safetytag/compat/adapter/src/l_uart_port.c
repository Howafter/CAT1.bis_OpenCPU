#include "l_uart_port.h"

extern const liot_uart_func_s liot_uart_pin_func[];
__attribute__((used)) static bool config_uart_bit_func(int uartPort)
{
    int uart_tx_bit     = 0;
    int uart_tx_func    = 0;
    int uart_rx_bit     = 0;
    int uart_rx_func    = 0;

    if(liot_uart_pin_func[uartPort].port == LIOT_PORT_NONE)
        return false;

    uart_tx_bit = liot_uart_pin_func[uartPort].tx_pin;
    uart_tx_func = liot_uart_pin_func[uartPort].tx_func;
    uart_rx_bit = liot_uart_pin_func[uartPort].rx_pin;
    uart_rx_func = liot_uart_pin_func[uartPort].rx_func;

    if (uart_tx_bit == LIOT_UART_PIN_NONE || uart_rx_bit == LIOT_UART_PIN_NONE ||
        uart_tx_func == LIOT_UART_FUNC_NONE || uart_rx_func == LIOT_UART_FUNC_NONE)
        return false;

    if (liot_pin_set_func(uart_tx_bit, uart_tx_func) != LIOT_GPIO_SUCCESS ||
        liot_pin_set_func(uart_rx_bit, uart_rx_func) != LIOT_GPIO_SUCCESS)
        return false;

    return true;
}

int l_uart_write(uart_port_config_t *config, void *data_ptr, int data_len) 
{
    if (config->inti_state == 0)
    {
        return -1;
    }

    unsigned char *data = data_ptr;
    return liot_uart_write(config->uart_port, data, data_len);
}

int l_uart_read(uart_port_config_t *config, void *data_ptr, int data_len) 
{
    if (config->inti_state == 0)
    {
        return -1;
    }

    return liot_uart_read(config->uart_port, data_ptr, data_len);
}

int l_uart_init(uart_port_config_t *config)
{
    int ret;

    if (config == NULL) {
        return -1;
    }
    if (config->inti_state != 0) {
        return LIOT_UART_SUCCESS;
    }
    if (!config_uart_bit_func(config->uart_port)) {
        return -1;
    }

    ret = liot_uart_set_dcbconfig(config->uart_port, &config->uart);
    if (ret != LIOT_UART_SUCCESS) {
        return ret;
    }

    ret = liot_uart_open(config->uart_port);
    if (ret != LIOT_UART_SUCCESS) {
        return ret;
    }

    ret = liot_uart_register_cb(config->uart_port, config->uart_cb);
    if (ret != LIOT_UART_SUCCESS) {
        (void)liot_uart_close(config->uart_port);
        return ret;
    }

    ret = liot_uart_tx_way_config(LIOT_UART_TX_DRIVER_DMA);
    if (ret != LIOT_UART_SUCCESS) {
        (void)liot_uart_register_cb(config->uart_port, NULL);
        (void)liot_uart_close(config->uart_port);
        return ret;
    }

    config->inti_state = 1;
    return LIOT_UART_SUCCESS;
}

int l_uart_deinit(uart_port_config_t *config)
{
    int ret;

    if (config == NULL) {
        return -1;
    }
    if (config->inti_state == 0) {
        return LIOT_UART_SUCCESS;
    }

    (void)liot_uart_register_cb(config->uart_port, NULL);
    ret = liot_uart_close(config->uart_port);
    if (ret == LIOT_UART_SUCCESS) {
        config->inti_state = 0;
    }
    return ret;
}