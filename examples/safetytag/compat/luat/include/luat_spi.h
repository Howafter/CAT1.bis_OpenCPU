#ifndef LUAT_SPI_H
#define LUAT_SPI_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SPI_ID0 = 0,
    SPI_ID1 = 1,
};

typedef struct {
    int id;
    int CPHA;
    int CPOL;
    int dataw;
    int bit_dict;
    int master;
    int mode;
    int bandrate;
    int cs;
} luat_spi_t;

int luat_spi_setup(luat_spi_t *conf);
int luat_spi_transfer(int id, const char *send_buf, size_t send_len, char *recv_buf, size_t recv_len);
int luat_spi_send(int id, const char *buf, size_t len);
int luat_spi_recv(int id, char *buf, size_t len);
int luat_spi_close(int id);

#ifdef __cplusplus
}
#endif

#endif
