#include "l_spi_port.h"
#include "l_gpio_port.h"

#define SPI_DEMO_CLK_SPEED          8000000U

#if (defined CHIP_EC718)
#define LIOT_CUR_SPI0_MOSI_PIN_MUN (85)
#define LIOT_CUR_SPI0_MISO_PIN_MUN (84)
#define LIOT_CUR_SPI0_CLK_PIN_MUN  (86)
#define LIOT_CUR_SPI0_CS0_PIN_MUN  (83)
#define LIOT_CUR_SPI_PIN_FUNC      (1)

#define LIOT_CUR_SPI1_MOSI_PIN_MUN (29)
#define LIOT_CUR_SPI1_MISO_PIN_MUN (58)
#define LIOT_CUR_SPI1_CLK_PIN_MUN  (57)
#define LIOT_CUR_SPI1_CS0_PIN_MUN  (28)

#elif (defined CHIP_EC716)
#define LIOT_CUR_SPI0_MOSI_PIN_MUN (51)
#define LIOT_CUR_SPI0_MISO_PIN_MUN (52)
#define LIOT_CUR_SPI0_CLK_PIN_MUN  (53)
#define LIOT_CUR_SPI0_CS0_PIN_MUN  (62)
#define LIOT_CUR_SPI_PIN_FUNC      (1)
#endif

#define SPI1_CS_GPIO   					LIOT_GPIO_23
#define SPI1_CS_GPIO_PIN           		(99)
#define SPI1_CS_GPIO_FUNC       	    (IO_FUNC_0)

void l_spi_port_init(spi_port_config_t *port_cfg)
{
    // io_config_t io_cfg = {
    //     .name = "spi1_cs",
    //     .inti_state = 0,
    //     .en_state = 1,
    //     .pin_num = SPI1_CS_GPIO_PIN,
    //     .func_sel = SPI1_CS_GPIO_FUNC,
    //     .gpio_num = SPI1_CS_GPIO,
    //     .gpio_dir = LIOT_GPIO_DIR_OUTPUT,
    //     .gpio_pull = LIOT_GPIO_PULL_NONE,
    //     .gpio_lvl = LIOT_GPIO_LVL_LOW,
    // };
    
    l_gpio_init(&port_cfg->cs_io_cfg);

    // liot_spi_config_s cfg = {
    //     .input_mode = LIOT_SPI_INPUT_TRUE,//SPI 输入功能
    //     .port = LIOT_SPI_PORT1,//SPI 总线编号
    //     .framesize = 8,//数据帧大小。范围：0～32；默认值：8；单位：bit
    //     .spiclk = SPI_DEMO_CLK_SPEED,//SPI 时钟频率
    //     .cs_polarity0 = LIOT_SPI_CS_ACTIVE_LOW,
    //     .cs_polarity1 = LIOT_SPI_CS_ACTIVE_LOW,
    //     .cpol = LIOT_SPI_CPOL_LOW,
    //     .cpha = LIOT_SPI_CPHA_1Edge,
    //     .input_sel = LIOT_SPI_DI_1,
    //     .transmode = LIOT_SPI_DIRECT_POLLING,//SPI 传输模式
    //     .cs = LIOT_SPI_CS_NULL,// 用户在外部控制CS引脚
    //     .clk_delay = LIOT_SPI_CLK_DELAY_0,
    //     .device_mode = LIOT_SPI_DEVICE_MODE_MASTER,
    //     .data_msb_lsb = LIOT_SPI_DATA_MSB_LSB, 
    //     .irq_callback = NULL,
    // };

    liot_pin_set_func(port_cfg->mosi_pin_num, port_cfg->mosi_func_sel);
    liot_pin_set_func(port_cfg->miso_pin_num, port_cfg->miso_func_sel);
    liot_pin_set_func(port_cfg->clk_pin_num, port_cfg->clk_func_sel);

    liot_spi_init_ext(port_cfg->spi_cfg);
}

int l_spi_write_read(spi_port_config_t *port_cfg, 
                            void *rxbuf,
                            void *txbuf,
                            unsigned int len)
{
    int ret = -1;
    unsigned char *inbuf = (unsigned char *)rxbuf;
    unsigned char *outbuf = (unsigned char *)txbuf;

    l_gpio_enable(&port_cfg->cs_io_cfg);
    ret = spi_write_read(port_cfg->spi_cfg.port, inbuf, outbuf, len);
    l_gpio_disable(&port_cfg->cs_io_cfg);

    return ret;
}

int l_spi_read(spi_port_config_t *port_cfg, 
                            unsigned char *outbuf,
                            unsigned int len)
{
    int ret = -1;
    l_gpio_enable(&port_cfg->cs_io_cfg);
    ret = spi_read(port_cfg->spi_cfg.port, outbuf, len);
    l_gpio_disable(&port_cfg->cs_io_cfg);
    
    return ret;
}

int l_spi_write(spi_port_config_t *port_cfg, 
                            unsigned char *inbuf,
                            unsigned int len)
{
    int ret = -1;
    l_gpio_enable(&port_cfg->cs_io_cfg);
    ret = spi_write(port_cfg->spi_cfg.port, inbuf, len);
    l_gpio_disable(&port_cfg->cs_io_cfg);
    
    return ret;
}

int l_spi_release(spi_port_config_t *port_cfg)
{
    int ret = -1;
   
    ret = spi_release(port_cfg->spi_cfg.port);

    return ret;
}