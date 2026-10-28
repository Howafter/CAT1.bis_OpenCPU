#ifndef LUAT_MOBILE_H
#define LUAT_MOBILE_H

#include <stddef.h>
#include <stdint.h>

#include "liot_dev.h"
#include "liot_nw.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int16_t rsrp;
    int16_t rsrq;
    int16_t snr;
    int16_t rssi;
} luat_mobile_lte_signal_strength_t;

typedef struct
{
    luat_mobile_lte_signal_strength_t lte_signal_strength;
} luat_mobile_signal_strength_info_t;

typedef struct
{
    int16_t rsrp;
    int16_t rsrq;
    int16_t snr;
    int16_t rssi;
} luat_mobile_lte_service_info_t;

typedef struct
{
    luat_mobile_lte_service_info_t lte_service_info;
} luat_mobile_cell_info_t;

static inline int16_t luat_mobile_convert_rsrp(int rsrp)
{
    if (rsrp == 127 || rsrp == 255)
    {
        return (int16_t)rsrp;
    }

    return (int16_t)((rsrp > 0) ? (rsrp - 141) : (rsrp - 140));
}

static inline int16_t luat_mobile_convert_rsrq(int rsrq)
{
    if (rsrq == 127 || rsrq == 255)
    {
        return (int16_t)rsrq;
    }

    if (rsrq <= 0)
    {
        return (int16_t)((rsrq - 39) >> 1);
    }

    if (rsrq <= 34)
    {
        return (int16_t)((rsrq - 40) >> 1);
    }

    return (int16_t)((rsrq - 41) >> 1);
}

static inline int16_t luat_mobile_convert_rssi(int rssi)
{
    if (rssi == 99)
    {
        return (int16_t)rssi;
    }

    return (int16_t)(-113 + (2 * rssi));
}

static inline int luat_mobile_get_imei(uint8_t sim_id, char *imei, size_t len)
{
    return (liot_dev_get_imei(imei, len, sim_id) == LIOT_DEV_SUCCESS) ? 0 : -1;
}

static inline int luat_mobile_get_last_notify_signal_strength_info(luat_mobile_signal_strength_info_t *info)
{
    liot_nw_signal_strength_info_s liot_info = {0};

    if (info == NULL)
    {
        return -1;
    }

    if (liot_nw_get_signal_strength(0, &liot_info) != LIOT_NW_SUCCESS)
    {
        return -1;
    }

    info->lte_signal_strength.rsrp = luat_mobile_convert_rsrp(liot_info.rsrp);
    info->lte_signal_strength.rsrq = luat_mobile_convert_rsrq(liot_info.rsrq);
    info->lte_signal_strength.snr = (int16_t)liot_info.snr;
    info->lte_signal_strength.rssi = luat_mobile_convert_rssi(liot_info.rssi);
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif
