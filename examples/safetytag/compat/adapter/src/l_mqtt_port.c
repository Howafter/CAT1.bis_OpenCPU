#include "l_mqtt_port.h"


#define MQTT_DEMO_ONENET_TEST 0
#define MQTT_DEMO_SSL_TEST  0

#define MAX_RECONNECT_BEFORE_REBOOT 10

// static int mqtt_connected = 0;
static volatile uint8_t datacall_flag = 0;
static volatile int8_t call_flag = -1;
// static liot_sem_t mqtt_semp;

// static int mqtt_global_reconnect_count = 0;//全局重连次数统计
/******************************************************************************************************
 * Function: datacall_start
 *
 * Description:
 *   启动数据拨号。默认为同步模式。拨号成功，获取到 IP 地址，可进行 socket 通信。
 *
 * Parameters:
 *
 * Return:
 *   LIOT_DATACALL_SUCCESS   成功
 *   other                   错误码
 *
 ******************************************************************************************************/
__attribute__((unused)) static int datacall_start()
{
    int ret         = 0;
    int profile_idx = 1;
    liot_data_call_info_t info;
    char ip4_addr_str[16] = {0};
    uint8_t nSim          = 0;

    if (datacall_flag == 1) return call_flag;
    datacall_flag = 1;
    call_flag = -1;

    liot_trace("===start data call begin====");
    liot_start_data_call(nSim, profile_idx, LIOT_DATA_TYPE_IP, "APNTEST", "", "", LIOT_DATA_AUTH_TYPE_NONE);
    liot_trace("===start data call end====");

    liot_rtos_task_sleep_s(5);
    ret = liot_get_data_call_info(nSim, profile_idx, &info);
    if (ret != 0)
    {
        liot_stop_data_call(nSim, profile_idx);
        liot_trace("liot_get_data_call_info ret: %d", ret);

        datacall_flag = 0;
        return ret;
    }
    liot_trace("info->cid: %d", info.cid);
    liot_trace("info->ip_version: %d", info.ip_version);

    liot_trace("info->v4.state: %d", info.v4.state);
    inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
    liot_trace("info.v4.addr.ip: %s", ip4_addr_str);

    inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
    liot_trace("info.v4.addr.pri_dns: %s", ip4_addr_str);

    inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
    liot_trace("info.v4.addr.sec_dns: %s", ip4_addr_str);

    call_flag = 0;
    return call_flag;
}

__attribute__((unused)) static void mqtt_connect_result_cb(liot_mqtt_client_t *client, void *arg, int status)
{
    mqtt_port_config_t *port_config = (mqtt_port_config_t *)arg;
    liot_trace("--%s--status: %d", port_config->name, status);
    if (status == 0)
    {
        port_config->mqtt_connected = 1;
    }

    // mqtt_global_reconnect_count = 0;
    port_config->reconnect_count = 0;

    liot_rtos_semaphore_release(port_config->mqtt_semp);
}

__attribute__((unused)) static void mqtt_state_exception_cb(liot_mqtt_client_t *client)
{
    liot_trace("mqtt session abnormal disconnect");
    // mqtt_connected = 0;
}

__attribute__((unused)) static void mqtt_inpub_data_cb(liot_mqtt_client_t *client,
                               void *arg,
                               int pkt_id,
                               const char *topic,
                               const unsigned char *payload,
                               unsigned short payload_len)
{
    liot_trace("topic: %s", topic);
    liot_trace("payload: %s", payload);
}

static void mqtt_requst_result_cb(liot_mqtt_client_t *client, void *arg, int err)
{
    mqtt_port_config_t *port_config = (mqtt_port_config_t *)arg;
    liot_trace("--%s--err: %d", port_config->name, err);

    liot_rtos_semaphore_release(port_config->mqtt_semp);
}

__attribute__((unused)) static void mqtt_disconnect_result_cb(liot_mqtt_client_t *client, void *arg, int err)
{
    mqtt_port_config_t *port_config = (mqtt_port_config_t *)arg;

    liot_trace("--%s--err: %d", port_config->name, err);
    liot_rtos_semaphore_release(port_config->mqtt_semp);
}

/**
 * @brief MQTT重连处理函数
 *
 * 当MQTT连接丢失时，此函数将被调用。它会输出一条跟踪信息，提示正在尝试重新连接。
 * 开发者可以在此函数中添加重连时的自定义处理逻辑。
 *
 * @param client 指向MQTT客户端实例的指针
 * @param arg 传递给回调函数的参数
 */
__attribute__((unused)) static void mqtt_reconnect_handler(liot_mqtt_client_t *client, void *arg)
{
    // 在这里可以添加重连时的处理逻辑
    mqtt_port_config_t *port_config = (mqtt_port_config_t *)arg;

    liot_trace("--%s--MQTT connection lost, trying to reconnect...", port_config->name);
    port_config->reconnect_count++;
    liot_trace("--%s--MQTT reconnect count: %d", port_config->name, port_config->reconnect_count);

    if (port_config->reconnect_count >= MAX_RECONNECT_BEFORE_REBOOT) {
        liot_trace("Critical: Maximum reconnect attempts (%d) reached, rebooting device", MAX_RECONNECT_BEFORE_REBOOT);
        // reboot_device();
        //TODO
    }
}

/******************************************************************************************************
 * Function: l_mqtt_client_init
 *
 * Description:
 *   初始化MQTT客户端。连接MQTT服务器，并设置相关回调函数。
 *
 * Parameters:
 *
 * Return:
 *   0   成功
 *   other                   错误码
 *
 ******************************************************************************************************/
int l_mqtt_client_init(mqtt_port_config_t *config)
{
    int ret                         = -1;
    liot_mqtt_client_t *mqtt_cli     = &config->client;
    liot_mqtt_client_option options = {0};
    int cid                         = 1;//默认值

    if (datacall_start() != 0)
    {
        liot_trace("--%s--data call failed", config->name);
        return ret;
    }

    int free_size = liot_xPortGetFreeHeapSize();
    liot_trace("free size :%d",free_size);

#if 0    
    struct liot_mqtt_ssl_config_t liot_ssl_cfg =
    {
        .ssl_ctx_id = 1,
#if MQTT_DEMO_SSL_TEST
        .verify_level = 1,
#else
        .verify_level = 2,
#endif
        .client_cert_type              = 1,
        .cacert_path                   = root_ca_crt_buffer,
        .client_cert_path              = client_crt_buffer,
        .client_key_path               = client_key_buffer,
        .client_key_pwd                = NULL,
        .ssl_version                   = 4,
        .sni_enable                    = 0,
        .ssl_negotiate_timeout         = 280,
        .ignore_invalid_certsign       = 0,
        .ignore_multi_certchain_verify = 0,
        // MBEDTLS_X509_BADCERT_NOT_TRUSTED|MBEDTLS_X509_BADCERT_EXPIRED|MBEDTLS_X509_BADCERT_FUTURE,
        .ignore_certitem = 0xfffff,
    };
#endif
    options.version = LIOT_MQTT_VERSION_4; // 3.1.1
    options.pdp_cid = 1;
#if MQTT_DEMO_SSL_TEST
    options.ssl_enable = true,
#else
    options.ssl_enable = false,
#endif
    options.ssl_cfg       = NULL;
    options.clean_session = 1;
    options.kalive_time   = 60;
    options.delivery_time = 5;
    options.delivery_cnt  = 3;
    options.will_flag     = 0;
    options.will_qos      = 0;
    options.will_retain   = 0;
    options.ping_timeout  = 5;
    options.client_id     = config->client_id;
    options.client_user   = config->username;
    options.client_pass   = config->password;
    memset(options.will_topic, 0x00, 256);
    memset(options.will_message, 0x00, 256);

    liot_rtos_semaphore_create(&config->mqtt_semp, 0);

    liot_trace("--%s--mqtt_client_init", config->name);

#if MQTT_DEMO_ONENET_TEST
    char *version = "2018-10-31";
    char *token   = NULL;

    // INT64 expire_time = liot_rtc_get_time_s() + (24 * 60 * 60 * 365);
    INT64 expire_time = 1724988071;
    liot_trace("expire_time:%ld", expire_time);

    token = liot_onenet_generate_auth_token(
        expire_time, options.client_user, (const char *)(options.client_id), version, options.client_pass);
    liot_trace("token:%s", token);
    liot_trace("token:%s", token + 64);

    if (NULL != token)
    {
        if (strlen(token) < 256)
        {
            options.client_pass = token;
            liot_trace("get token successfully");
        }
        else
        {
            liot_trace("token error");
        }
    }

#endif

    if (liot_mqtt_client_init(mqtt_cli, cid) != LIOT_MQTTCLIENT_SUCCESS)
    {
        liot_trace("--%s--mqtt client init failed!!!!", config->name);
    }

        // 设置重连回调
    liot_mqtt_set_reconnect_callback(mqtt_cli, mqtt_reconnect_handler, config);

    // liot_trace("ssl &liot_ssl_cfg:%p", &liot_ssl_cfg);
    // options.ssl_cfg = &liot_ssl_cfg;
    
    char url[50] = {0};
    char port[16] = {0};
    itoa(config->port, port, 10);
    snprintf(url, sizeof(url), "%s:%s", config->host, port);

    ret = liot_mqtt_connect(mqtt_cli,
                            url,
                            mqtt_connect_result_cb,
                            config,
                            (liot_mqtt_client_option *)&options,
                            mqtt_state_exception_cb);
    if (ret == LIOT_MQTTCLIENT_WOUNDBLOCK)
    {
        liot_trace("--%s--wait connect result", config->name);
        liot_rtos_semaphore_wait(config->mqtt_semp, LIOT_WAIT_FOREVER);
        liot_trace("--%s--wait connect ok", config->name);
        if (config->mqtt_connected == 0)
        {
            liot_mqtt_client_deinit(mqtt_cli);
        }
    }
    else
    {
        liot_trace("--%s--mqtt connect failed ,ret = %d", config->name, ret);
    }

    liot_mqtt_set_inpub_callback(mqtt_cli, config->inpub_cb, NULL);

    return 0;
}

int l_mqtt_client_deinit(mqtt_port_config_t *config)
{
    if (config->mqtt_connected == 1 &&
        liot_mqtt_disconnect(&config->client, mqtt_disconnect_result_cb, config) == LIOT_MQTTCLIENT_WOUNDBLOCK)
    {
        liot_trace("--%s--wait disconnect result", config->name);
        liot_rtos_semaphore_wait(config->mqtt_semp, LIOT_WAIT_FOREVER);
        liot_trace("--%s--wait disconnect ok", config->name);
    }
    
    liot_rtos_task_sleep_s(5);

    liot_mqtt_client_deinit(&config->client);
    config->mqtt_connected = 0;

    liot_rtos_semaphore_delete(config->mqtt_semp);
    config->mqtt_semp = NULL;

    return 0;
}

int l_mqtt_sub_unsub(mqtt_port_config_t *config,
                        const char *topic,
                        unsigned char qos,
                        unsigned char sub)
{
    if (liot_mqtt_sub_unsub(&config->client, topic, qos, mqtt_requst_result_cb, config, sub) ==
        LIOT_MQTTCLIENT_WOUNDBLOCK)
    {
        liot_trace("--%s--wait subscrible result", config->name);
        liot_rtos_semaphore_wait(config->mqtt_semp, LIOT_WAIT_FOREVER);
        liot_trace("--%s--wait subscrible ok", config->name);
    }

    return 0;
}

int l_mqtt_publish(mqtt_port_config_t *config,
                      const char *topic,
                      const void *payload,
                      unsigned short payload_length,
                      unsigned char qos,
                      unsigned char retain)
{
    int ret = 0;
    ret = liot_mqtt_client_is_connected(&config->client);
    liot_trace("--%s--ret: %d", config->name, ret);
    if (ret == 1)
    {
        if (liot_mqtt_publish(&config->client,
                            topic,
                            payload,
                            payload_length,
                            qos,
                            retain,
                            mqtt_requst_result_cb,
                            config) == LIOT_MQTTCLIENT_WOUNDBLOCK)
        {
            liot_trace("--%s--wait publish result", config->name);
            liot_rtos_semaphore_wait(config->mqtt_semp, LIOT_WAIT_FOREVER);
            liot_trace("--%s--wait publish ok", config->name);
        }
    }

    return ret;
}


