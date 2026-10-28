#ifndef __L_SPI_PORT_H__
#define __L_SPI_PORT_H__

#include "lierda_api.h"
#include "l_gpio_port.h"


typedef struct
{
    io_config_t cs_io_cfg;

    liot_spi_config_s spi_cfg;

    uint8_t mosi_pin_num,
            miso_pin_num,
            clk_pin_num;
    uint8_t mosi_func_sel,
            miso_func_sel,
            clk_func_sel;

}spi_port_config_t;


void l_spi_port_init(spi_port_config_t *port_cfg);
int l_spi_write_read(spi_port_config_t *port_cfg, 
                            void *rxbuf,
                            void *txbuf,
                            unsigned int len);
int l_spi_release(spi_port_config_t *port_cfg);

#endif // __L_SPI_PORT_H__