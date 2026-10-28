#include "luat_spi.h"

#include <string.h>

#include "liot_spi.h"

static liot_spi_clk_e luat_to_liot_spi_clk(int bandrate)
{
    if (bandrate <= 812500)
    {
        return LIOT_SPI_CLK_812_5KHZ;
    }
    if (bandrate <= 1625000)
    {
        return LIOT_SPI_CLK_1_625MHZ;
    }
    if (bandrate <= 3250000)
    {
        return LIOT_SPI_CLK_3_25MHZ;
    }
    if (bandrate <= 6500000)
    {
        return LIOT_SPI_CLK_6_5MHZ;
    }
    return LIOT_SPI_CLK_13MHZ;
}

int luat_spi_setup(luat_spi_t *conf)
{
    liot_spi_config_s cfg;

    if (conf == NULL)
    {
        return -1;
    }

    memset(&cfg, 0, sizeof(cfg));

    cfg.port = (liot_spi_port_e)conf->id;
    cfg.framesize = (unsigned int)conf->dataw;
    cfg.spiclk = luat_to_liot_spi_clk(conf->bandrate);
    cfg.cpol = conf->CPOL ? LIOT_SPI_CPOL_HIGH : LIOT_SPI_CPOL_LOW;
    cfg.cpha = conf->CPHA ? LIOT_SPI_CPHA_2Edge : LIOT_SPI_CPHA_1Edge;
    cfg.transmode = LIOT_SPI_DIRECT_POLLING;
    cfg.input_mode = LIOT_SPI_INPUT_TRUE;
    cfg.input_sel = LIOT_SPI_DI_1;
    cfg.clk_delay = LIOT_SPI_CLK_DELAY_0;
    cfg.data_msb_lsb = conf->bit_dict ? LIOT_SPI_DATA_MSB_LSB : LIOT_SPI_DATA_LSB_MSB;
    cfg.cs = (conf->cs < 0) ? LIOT_SPI_CS_NULL : LIOT_SPI_CS0;
    cfg.cs_polarity0 = LIOT_SPI_CS_ACTIVE_LOW;
    cfg.cs_polarity1 = LIOT_SPI_CS_ACTIVE_LOW;
    cfg.device_mode = conf->master
                          ? ((conf->mode == 1) ? LIOT_SPI_DEVICE_MODE_MASTER : LIOT_SPI_DEVICE_MODE_MASTER_SIMPLEX)
                          : ((conf->mode == 1) ? LIOT_SPI_DEVICE_MODE_SLAVE : LIOT_SPI_DEVICE_MODE_SLAVE_SIMPLEX);
    cfg.irq_callback = NULL;

    return (int)liot_spi_init_ext(cfg);
}

int luat_spi_transfer(int id, const char *send_buf, size_t send_len, char *recv_buf, size_t recv_len)
{
    size_t same_len;
    int ret;

    if (send_buf != NULL && recv_buf != NULL && send_len == recv_len)
    {
        return (int)liot_spi_write_read((liot_spi_port_e)id,
                                        (unsigned char *)recv_buf,
                                        (unsigned char *)send_buf,
                                        (unsigned int)send_len);
    }

    same_len = (send_len < recv_len) ? send_len : recv_len;
    if (send_buf != NULL && recv_buf != NULL && same_len > 0)
    {
        ret = (int)liot_spi_write_read((liot_spi_port_e)id,
                                       (unsigned char *)recv_buf,
                                       (unsigned char *)send_buf,
                                       (unsigned int)same_len);
        if (ret != 0)
        {
            return ret;
        }
    }

    if (send_buf != NULL && send_len > same_len)
    {
        ret = (int)liot_spi_write((liot_spi_port_e)id,
                                  (unsigned char *)(send_buf + same_len),
                                  (unsigned int)(send_len - same_len));
        if (ret != 0)
        {
            return ret;
        }
    }

    if (recv_buf != NULL && recv_len > same_len)
    {
        ret = (int)liot_spi_read((liot_spi_port_e)id,
                                 (unsigned char *)(recv_buf + same_len),
                                 (unsigned int)(recv_len - same_len));
        if (ret != 0)
        {
            return ret;
        }
    }

    return 0;
}

int luat_spi_send(int id, const char *buf, size_t len)
{
    if (buf == NULL)
    {
        return -1;
    }

    return (int)liot_spi_write((liot_spi_port_e)id, (unsigned char *)buf, (unsigned int)len);
}

int luat_spi_recv(int id, char *buf, size_t len)
{
    if (buf == NULL)
    {
        return -1;
    }

    return (int)liot_spi_read((liot_spi_port_e)id, (unsigned char *)buf, (unsigned int)len);
}

int luat_spi_close(int id)
{
    return (int)liot_spi_release((liot_spi_port_e)id);
}
