#ifndef __LUAT_UART_H__
#define __LUAT_UART_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Virtual UART ID (USB CDC/ACM virtual COM) */
#define LUAT_VUART_ID_0  LIOT_USB_PORT_COM   /* matches LIOT_USB_PORT_COM enum value */

/* luat_uart_ctrl() commands */
#define LUAT_UART_SET_RECV_CALLBACK  0
#define LUAT_UART_SET_SENT_CALLBACK  1

/* Receive callback signature: void cb(int uart_id, uint32_t data_len) */
typedef void (*luat_uart_recv_callback_t)(int uart_id, uint32_t data_len);

typedef struct {
    int id;          /* UART port number */
    int baud_rate;   /* baud rate, e.g. 115200 */
    int data_bits;   /* 7 or 8 */
    int stop_bits;   /* 1 or 2 */
    int parity;      /* 0=none, 1=odd, 2=even */
    int bit_order;   /* unused – reserved */
    int bufsz;       /* unused – reserved */
} luat_uart_t;

/**
 * Optional pre-setup hook (pin-mux etc.). Currently no-op.
 */
int luat_uart_pre_setup(int id, int param);

/**
 * Configure and open a UART port.
 * @return 0 on success, negative on error.
 */
int luat_uart_setup(luat_uart_t *uart);

/**
 * Close a UART port and release its receive callback.
 */
int luat_uart_close(int id);

/**
 * Write data to a UART port.
 * @return number of bytes written, or negative on error.
 */
int luat_uart_write(int id, const void *data, size_t len);

/**
 * Read data from a UART port.
 * @return number of bytes read, or negative on error.
 */
int luat_uart_read(int id, void *buf, size_t len);

/**
 * Control a UART port (e.g. register callbacks).
 *
 * cmd == LUAT_UART_SET_RECV_CALLBACK:
 *   param must be cast-compatible with luat_uart_recv_callback_t.
 *
 * @return 0 on success, negative on error.
 */
int luat_uart_ctrl(int id, int cmd, void *param);

#ifdef __cplusplus
}
#endif

#endif /* __LUAT_UART_H__ */
