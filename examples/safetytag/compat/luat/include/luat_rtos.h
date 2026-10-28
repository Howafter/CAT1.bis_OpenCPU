#ifndef LUAT_RTOS_H
#define LUAT_RTOS_H

#include <stddef.h>
#include <stdint.h>

#include "liot_os.h"
#include "luat_init_export.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef liot_task_t luat_rtos_task_handle;
typedef liot_queue_t luat_rtos_queue_t;
typedef liot_mutex_t luat_rtos_mutex_t;
typedef liot_sem_t luat_rtos_semaphore_t;
typedef liot_timer_t luat_rtos_timer_t;
typedef liot_flag_t luat_rtos_flag_t;
typedef liot_flag_op_e LUAT_FLAG_OP_E;
typedef void (*luat_rtos_task_entry_t)(void *param);
typedef void (*luat_rtos_timer_callback_t)(void *param);

typedef struct
{
    uint32_t event_id;
    uint32_t param1;
    uint32_t param2;
    uint32_t param3;
} luat_event_t;

typedef int LUAT_RT_RET_TYPE;
typedef LUAT_RT_RET_TYPE (*luat_rtos_event_wait_callback_t)(void *);

#define LUAT_WAIT_FOREVER LIOT_WAIT_FOREVER
#define LUAT_NO_WAIT LIOT_NO_WAIT

int luat_rtos_task_create(luat_rtos_task_handle *task_handle,
                          uint32_t stack_size,
                          uint8_t priority,
                          const char *task_name,
                          luat_rtos_task_entry_t task_entry,
                          void *param,
                          void *reserved);
int luat_rtos_task_delete(luat_rtos_task_handle task_handle);
int luat_rtos_task_suspend(luat_rtos_task_handle task_handle);
int luat_rtos_task_resume(luat_rtos_task_handle task_handle);
void luat_rtos_task_suspend_all(void);
void luat_rtos_task_resume_all(void);
luat_rtos_task_handle luat_rtos_get_current_handle(void);
uint32_t luat_rtos_task_get_high_water_mark(luat_rtos_task_handle task_handle);
void luat_rtos_task_sleep(uint32_t ms);

int luat_rtos_mutex_create(luat_rtos_mutex_t *mutex_handle);
int luat_rtos_mutex_lock(luat_rtos_mutex_t mutex_handle, uint32_t timeout);
int luat_rtos_mutex_unlock(luat_rtos_mutex_t mutex_handle);
int luat_rtos_mutex_delete(luat_rtos_mutex_t mutex_handle);

int luat_rtos_queue_create(luat_rtos_queue_t *queue_handle, uint32_t msg_count, uint32_t msg_size);
int luat_rtos_queue_send(luat_rtos_queue_t queue_handle, void *data, uint32_t msg_size, uint32_t timeout);
int luat_rtos_queue_recv(luat_rtos_queue_t queue_handle, void *data, uint32_t *msg_size, uint32_t timeout);
int luat_rtos_queue_get_cnt(luat_rtos_queue_t queue_handle, uint32_t *cnt);
int luat_rtos_queue_delete(luat_rtos_queue_t queue_handle);

int luat_rtos_semaphore_create(luat_rtos_semaphore_t *semaphore_handle, uint32_t initial_count);
int luat_rtos_semaphore_delete(luat_rtos_semaphore_t semaphore_handle);
int luat_rtos_semaphore_take(luat_rtos_semaphore_t semaphore_handle, uint32_t timeout);
int luat_rtos_semaphore_release(luat_rtos_semaphore_t semaphore_handle);

int luat_rtos_flag_create(luat_rtos_flag_t *flag_handle);
int luat_rtos_flag_wait(luat_rtos_flag_t flag_handle,
                        uint32_t mask,
                        LUAT_FLAG_OP_E operation,
                        uint32_t *flags,
                        uint32_t timeout);
int luat_rtos_flag_release(luat_rtos_flag_t flag_handle, uint32_t mask, LUAT_FLAG_OP_E operation);
int luat_rtos_flag_delete(luat_rtos_flag_t flag_handle);

int luat_rtos_timer_create(luat_rtos_timer_t *timer_handle);
int luat_rtos_timer_start(luat_rtos_timer_t timer_handle,
                          uint32_t timeout,
                          uint8_t repeat,
                          luat_rtos_timer_callback_t callback_fun,
                          void *user_param);
int luat_rtos_timer_stop(luat_rtos_timer_t timer_handle);
int luat_rtos_timer_is_active(luat_rtos_timer_t timer_handle);
int luat_rtos_timer_delete(luat_rtos_timer_t timer_handle);

int luat_start_rtos_timer(luat_rtos_timer_t timer_handle,
                          uint32_t timeout,
                          uint8_t repeat,
                          luat_rtos_timer_callback_t callback_fun,
                          void *user_param);
int luat_stop_rtos_timer(luat_rtos_timer_t timer_handle);

int luat_rtos_event_send(luat_rtos_task_handle task_handle,
                         uint32_t id,
                         uint32_t param1,
                         uint32_t param2,
                         uint32_t param3,
                         uint32_t timeout);
int luat_rtos_event_recv(luat_rtos_task_handle task_handle,
                         uint32_t wait_event_id,
                         luat_event_t *out_event,
                         luat_rtos_event_wait_callback_t *callback_fun,
                         uint32_t timeout);
int luat_rtos_message_send(luat_rtos_task_handle task_handle, uint32_t message_id, void *p_message);
int luat_rtos_message_recv(luat_rtos_task_handle task_handle,
                           uint32_t *message_id,
                           void **p_p_message,
                           uint32_t timeout);

uint32_t luat_rtos_entry_critical(void);
void luat_rtos_exit_critical(uint32_t critical_token);

#ifdef __cplusplus
}
#endif

#endif