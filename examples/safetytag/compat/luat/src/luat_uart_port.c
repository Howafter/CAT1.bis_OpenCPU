#include "luat_uart.h"

#include <string.h>

#include "liot_uart.h"

/* Per-port Luat-style receive callbacks. One entry per liot_uart_port_number_e. */
static luat_uart_recv_callback_t g_luat_uart_recv_cbs[LIOT_PORT_MAX];

/* Lierda callback adapter: convert Lierda signature to Luat signature. */
static void luat_uart_cb_adapter(uint32_t ind_type,
                                  liot_uart_port_number_e port,
                                  uint32_t size)
{
    if ((ind_type & (uint32_t)LIOT_UART_EVENT_RX_ARRIVED) == 0)
    {
        return;
    }

    if ((int)port < 0 || (int)port >= (int)LIOT_PORT_MAX)
    {
        return;
    }

    if (g_luat_uart_recv_cbs[port] != NULL)
    {
        g_luat_uart_recv_cbs[port]((int)port, (uint32_t)size);
    }
}

int luat_uart_pre_setup(int id, int param)
{
    (void)id;
    (void)param;
    return 0;
}

int luat_uart_setup(luat_uart_t *uart)
{
    liot_uart_port_number_e port;
    liot_uart_config_s dcb;
    int ret;

    if (uart == NULL)
    {
        return -1;
    }

    port = (liot_uart_port_number_e)uart->id;

    /* USB virtual COM does not need DCB configuration */
    if (port == LIOT_USB_PORT_COM)
    {
        return (int)liot_uart_open(port);
    }

    memset(&dcb, 0, sizeof(dcb));
    dcb.baudrate   = (liot_uart_baud_e)uart->baud_rate;
    dcb.data_bit   = (liot_uart_databit_e)uart->data_bits;
    dcb.stop_bit   = (liot_uart_stopbit_e)uart->stop_bits;
    dcb.parity_bit = (liot_uart_paritybit_e)uart->parity;
    dcb.flow_ctrl  = LIOT_FC_NONE;
    dcb.isPrintfPort = 0;

    ret = (int)liot_uart_set_dcbconfig(port, &dcb);
    if (ret != 0)
    {
        return ret;
    }

    return (int)liot_uart_open(port);
}

int luat_uart_close(int id)
{
    liot_uart_port_number_e port = (liot_uart_port_number_e)id;

    if ((int)port >= 0 && (int)port < (int)LIOT_PORT_MAX)
    {
        g_luat_uart_recv_cbs[port] = NULL;
    }

    return (int)liot_uart_close(port);
}

int luat_uart_write(int id, const void *data, size_t len)
{
    if (data == NULL || len == 0)
    {
        return -1;
    }

    return (int)liot_uart_write((liot_uart_port_number_e)id,
                                (unsigned char *)data,
                                (unsigned int)len);
}

int luat_uart_read(int id, void *buf, size_t len)
{
    if (buf == NULL || len == 0)
    {
        return -1;
    }

    return (int)liot_uart_read((liot_uart_port_number_e)id,
                               (unsigned char *)buf,
                               (unsigned int)len);
}

int luat_uart_ctrl(int id, int cmd, void *param)
{
    liot_uart_port_number_e port = (liot_uart_port_number_e)id;

    if (cmd == LUAT_UART_SET_RECV_CALLBACK)
    {
        if ((int)port < 0 || (int)port >= (int)LIOT_PORT_MAX)
        {
            return -1;
        }

        /* Store the Luat-style callback and register our adapter with Lierda */
        g_luat_uart_recv_cbs[port] = (luat_uart_recv_callback_t)param;
        return (int)liot_uart_register_cb(port, luat_uart_cb_adapter);
    }

    /* LUAT_UART_SET_SENT_CALLBACK: TX-complete callback not currently bridged */
    return 0;
}
