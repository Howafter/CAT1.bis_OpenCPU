#ifndef LUAT_I2C_H
#define LUAT_I2C_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    I2C_ID0 = 0,
    I2C_ID1 = 1,
    I2C_ID2 = 2,
};

int luat_i2c_setup(int id, int speed);
int luat_i2c_send(int id, int slave_addr, const uint8_t *data, size_t len, int stop);
int luat_i2c_recv(int id, int slave_addr, uint8_t *buf, size_t len);
int luat_i2c_close(int id);
int luat_i2c_set_iomux(int id, uint8_t value);//todo：luat_i2c_set_iomux 目前是 no-op，需要根据实际情况实现引脚复用设置

#ifdef __cplusplus
}
#endif

#endif
