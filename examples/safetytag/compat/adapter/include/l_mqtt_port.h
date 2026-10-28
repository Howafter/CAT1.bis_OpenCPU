#ifndef __L_MQTT_PORT_H__
#define __L_MQTT_PORT_H__

#include "lierda_api.h"

typedef struct
{
    char *name;
    liot_mqtt_client_t client;
    liot_mqtt_incoming_publish_cb_t inpub_cb;

    // char *product_id;
    char host[64];//mqtt 通信对端IP地址或域名地址.
    int port;
    char *client_id;
    char *username;//CONNECT 请求时携带的用户名
    char *password;

    uint8_t mqtt_connected;
    int reconnect_count;

    liot_sem_t mqtt_semp;

} mqtt_port_config_t;

int l_mqtt_publish(mqtt_port_config_t *config,
                      const char *topic,
                      const void *payload,
                      unsigned short payload_length,
                      unsigned char qos,
                      unsigned char retain);

int l_mqtt_sub_unsub(mqtt_port_config_t *config,
                        const char *topic,
                        unsigned char qos,
                        unsigned char sub);
                                              
int l_mqtt_client_init(mqtt_port_config_t *config);
int l_mqtt_client_deinit(mqtt_port_config_t *config);

#endif // __L_MQTT_PORT_H__