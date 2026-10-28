#include "luat_i2c.h"

#include "liot_i2c.h"

static uint8_t g_luat_i2c_last_reg[3] = {0};

static liot_i2c_channel_e luat_to_liot_i2c_ch(int id)
{
    switch (id)
    {
        case 0:
            return liot_i2c_1;
        case 1:
            return liot_i2c_2;
        case 2:
            return liot_i2c_3;
        default:
            return liot_i2c_1;
    }
}

int luat_i2c_setup(int id, int speed)
{
    liot_i2c_mode_e mode = (speed >= 1) ? LIOT_FAST_MODE : LIOT_STANDARD_MODE;
    return (int)liot_I2cInit(luat_to_liot_i2c_ch(id), mode);
}

int luat_i2c_send(int id, int slave_addr, const uint8_t *data, size_t len, int stop)
{
    liot_i2c_channel_e ch;
    uint8_t slave;

    (void)stop;

    if (data == NULL || len == 0)
    {
        return -1;
    }

    ch = luat_to_liot_i2c_ch(id);
    slave = (uint8_t)(slave_addr & 0x7F);

    if (len == 1)
    {
        if (id >= 0 && id < 3)
        {
            g_luat_i2c_last_reg[id] = data[0];
        }
        return (int)liot_I2cWrite(ch, slave, data[0], NULL, 0);
    }

    if (id >= 0 && id < 3)
    {
        g_luat_i2c_last_reg[id] = data[0];
    }

    return (int)liot_I2cWrite(ch, slave, data[0], (uint8_t *)&data[1], (uint32_t)(len - 1));
}

int luat_i2c_recv(int id, int slave_addr, uint8_t *buf, size_t len)
{
    liot_i2c_channel_e ch;
    uint8_t slave;
    uint8_t reg_addr = 0;

    if (buf == NULL || len == 0)
    {
        return -1;
    }

    ch = luat_to_liot_i2c_ch(id);
    slave = (uint8_t)(slave_addr & 0x7F);

    if (id >= 0 && id < 3)
    {
        reg_addr = g_luat_i2c_last_reg[id];
    }

    return (int)liot_I2cRead(ch, slave, reg_addr, buf, (uint32_t)len);
}

int luat_i2c_close(int id)
{
    return (int)liot_I2cRelease(luat_to_liot_i2c_ch(id));
}

int luat_i2c_set_iomux(int id, uint8_t value)
{
    (void)id;
    (void)value;
    return 0;
}
