#include "luat_rtos.h"

#define LUAT_RTOS_OK 0
#define LUAT_RTOS_TIMER_CTX_MAX 32
#define LUAT_RTOS_MAILBOX_MAX 16
#define LUAT_RTOS_EVENT_QUEUE_DEPTH 16
#define LUAT_RTOS_MSG_QUEUE_DEPTH 16
#define LUAT_RTOS_POLL_SLICE_MS 10U

typedef struct
{
    uint8_t used;
    uint8_t repeat;
    uint8_t active;
    uint32_t timeout;
    luat_rtos_timer_t timer;
    luat_rtos_timer_callback_t callback;
    void *user_param;
} luat_rtos_timer_ctx_t;

typedef struct
{
    luat_rtos_task_handle task_handle;
    uint32_t message_id;
    void *message_ptr;
} luat_rtos_msg_t;

typedef struct
{
    uint8_t used;
    luat_rtos_task_handle task_handle;
    luat_rtos_queue_t event_queue;
    luat_rtos_queue_t msg_queue;
} luat_rtos_mailbox_t;

static luat_rtos_timer_ctx_t g_luat_timer_ctx[LUAT_RTOS_TIMER_CTX_MAX];
static luat_rtos_mailbox_t g_luat_mailbox[LUAT_RTOS_MAILBOX_MAX];
static uint32_t g_luat_suspend_all_nest;
static uint32_t g_luat_suspend_all_token;

static int luat_status_from_liot(LiotOSStatus_t status)
{
    return status == LUAT_RTOS_OK ? 0 : -1;
}

static uint32_t luat_rtos_normalize_msg_size(uint32_t msg_size)
{
    return msg_size == 0U ? (uint32_t)sizeof(void *) : msg_size;
}

static int luat_rtos_is_in_isr(void)
{
    return __get_IPSR() != 0U;

}

static luat_rtos_timer_ctx_t *luat_rtos_timer_ctx_find(luat_rtos_timer_t timer)
{
    uint32_t i;

    for (i = 0; i < LUAT_RTOS_TIMER_CTX_MAX; i++)
    {
        if (g_luat_timer_ctx[i].used && g_luat_timer_ctx[i].timer == timer)
        {
            return &g_luat_timer_ctx[i];
        }
    }

    return NULL;
}

static luat_rtos_timer_ctx_t *luat_rtos_timer_ctx_alloc(void)
{
    uint32_t i;

    for (i = 0; i < LUAT_RTOS_TIMER_CTX_MAX; i++)
    {
        if (!g_luat_timer_ctx[i].used)
        {
            g_luat_timer_ctx[i].used = 1U;
            g_luat_timer_ctx[i].repeat = 0U;
            g_luat_timer_ctx[i].active = 0U;
            g_luat_timer_ctx[i].timeout = 0U;
            g_luat_timer_ctx[i].timer = NULL;
            g_luat_timer_ctx[i].callback = NULL;
            g_luat_timer_ctx[i].user_param = NULL;
            return &g_luat_timer_ctx[i];
        }
    }

    return NULL;
}

static void luat_rtos_timer_ctx_free(luat_rtos_timer_ctx_t *ctx)
{
    if (ctx == NULL)
    {
        return;
    }

    ctx->used = 0U;
    ctx->repeat = 0U;
    ctx->active = 0U;
    ctx->timeout = 0U;
    ctx->timer = NULL;
    ctx->callback = NULL;
    ctx->user_param = NULL;
}

static void luat_rtos_timer_dispatch(void *arg)
{
    luat_rtos_timer_ctx_t *ctx = (luat_rtos_timer_ctx_t *)arg;

    if (ctx == NULL || !ctx->used)
    {
        return;
    }

    if (ctx->callback != NULL)
    {
        ctx->callback(ctx->user_param);
    }

    if (ctx->repeat)
    {
        /* For periodic timer, restart it automatically */
        if (luat_rtos_is_in_isr())
        {
            (void)liot_rtos_timer_start_isr(ctx->timer, ctx->timeout);
        }
        else
        {
            (void)liot_rtos_timer_start(ctx->timer, ctx->timeout);
        }
    }
    else
    {
        /* For one-shot timer, mark as inactive */
        ctx->active = 0U;
    }
}

static luat_rtos_mailbox_t *luat_rtos_mailbox_find(luat_rtos_task_handle task_handle)
{
    uint32_t i;

    for (i = 0; i < LUAT_RTOS_MAILBOX_MAX; i++)
    {
        if (g_luat_mailbox[i].used && g_luat_mailbox[i].task_handle == task_handle)
        {
            return &g_luat_mailbox[i];
        }
    }

    return NULL;
}

static luat_rtos_mailbox_t *luat_rtos_mailbox_alloc(luat_rtos_task_handle task_handle)
{
    uint32_t i;

    for (i = 0; i < LUAT_RTOS_MAILBOX_MAX; i++)
    {
        if (!g_luat_mailbox[i].used)
        {
            g_luat_mailbox[i].used = 1U;
            g_luat_mailbox[i].task_handle = task_handle;
            g_luat_mailbox[i].event_queue = NULL;
            g_luat_mailbox[i].msg_queue = NULL;
            return &g_luat_mailbox[i];
        }
    }

    return NULL;
}

static luat_rtos_mailbox_t *luat_rtos_mailbox_get(luat_rtos_task_handle task_handle, int create_if_missing)
{
    luat_rtos_mailbox_t *mb;

    if (task_handle == NULL)
    {
        return NULL;
    }

    mb = luat_rtos_mailbox_find(task_handle);
    if (mb != NULL || !create_if_missing)
    {
        return mb;
    }

    return luat_rtos_mailbox_alloc(task_handle);
}

static int luat_rtos_mailbox_ensure_event_queue(luat_rtos_mailbox_t *mb)
{
    if (mb == NULL)
    {
        return -1;
    }

    if (mb->event_queue != NULL)
    {
        return 0;
    }

    return luat_status_from_liot(liot_rtos_queue_create(&mb->event_queue,
                                                        sizeof(luat_event_t),
                                                        LUAT_RTOS_EVENT_QUEUE_DEPTH));
}

static int luat_rtos_mailbox_ensure_msg_queue(luat_rtos_mailbox_t *mb)
{
    if (mb == NULL)
    {
        return -1;
    }

    if (mb->msg_queue != NULL)
    {
        return 0;
    }

    return luat_status_from_liot(liot_rtos_queue_create(&mb->msg_queue,
                                                        sizeof(luat_rtos_msg_t),
                                                        LUAT_RTOS_MSG_QUEUE_DEPTH));
}

int luat_rtos_task_create(luat_rtos_task_handle *task_handle,
                          uint32_t stack_size,
                          uint8_t priority,
                          const char *task_name,
                          luat_rtos_task_entry_t task_entry,
                          void *param,
                          void *reserved)
{
    (void)reserved;
    if (task_handle == NULL || task_entry == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_task_create(task_handle,
                                                       stack_size,
                                                       priority,
                                                       (char *)task_name,
                                                       task_entry,
                                                       param,
                                                       NULL));
}

int luat_rtos_task_delete(luat_rtos_task_handle task_handle)
{
    if (task_handle == NULL)
    {
        task_handle = luat_rtos_get_current_handle();
    }

    if (task_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_task_delete(task_handle));
}

int luat_rtos_task_suspend(luat_rtos_task_handle task_handle)
{
    return luat_status_from_liot(liot_rtos_task_suspend(task_handle));
}

int luat_rtos_task_resume(luat_rtos_task_handle task_handle)
{
    return luat_status_from_liot(liot_rtos_task_resume(task_handle));
}

void luat_rtos_task_suspend_all(void)
{
    uint32_t token;

    token = luat_rtos_entry_critical();
    if (g_luat_suspend_all_nest == 0U)
    {
        g_luat_suspend_all_token = token;
    }
    g_luat_suspend_all_nest++;
}

void luat_rtos_task_resume_all(void)
{
    if (g_luat_suspend_all_nest == 0U)
    {
        return;
    }

    g_luat_suspend_all_nest--;
    if (g_luat_suspend_all_nest == 0U)
    {
        luat_rtos_exit_critical(g_luat_suspend_all_token);
        g_luat_suspend_all_token = 0U;
    }
}

luat_rtos_task_handle luat_rtos_get_current_handle(void)
{
    luat_rtos_task_handle task_handle = NULL;

    if (luat_status_from_liot(liot_rtos_task_get_current_ref(&task_handle)) != 0)
    {
        return NULL;
    }

    return task_handle;
}

uint32_t luat_rtos_task_get_high_water_mark(luat_rtos_task_handle task_handle)
{
    if (task_handle == NULL)
    {
        return 0U;
    }

    return liot_rtos_task_get_stack_space(task_handle);
}

void luat_rtos_task_sleep(uint32_t ms)
{
    liot_rtos_task_sleep_ms(ms);
}

int luat_rtos_mutex_create(luat_rtos_mutex_t *mutex_handle)
{
    if (mutex_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_mutex_create(mutex_handle));
}

int luat_rtos_mutex_lock(luat_rtos_mutex_t mutex_handle, uint32_t timeout)
{
    return luat_status_from_liot(liot_rtos_mutex_lock(mutex_handle, timeout));
}

int luat_rtos_mutex_unlock(luat_rtos_mutex_t mutex_handle)
{
    return luat_status_from_liot(liot_rtos_mutex_unlock(mutex_handle));
}

int luat_rtos_mutex_delete(luat_rtos_mutex_t mutex_handle)
{
    return luat_status_from_liot(liot_rtos_mutex_delete(mutex_handle));
}

int luat_rtos_queue_create(luat_rtos_queue_t *queue_handle, uint32_t msg_count, uint32_t msg_size)
{
    if (queue_handle == NULL || msg_size == 0U || msg_count == 0U)
    {
        return -1;
    }

    /* Luat accepts (message count, message size); LIOT expects
     * (maximum message size in bytes, maximum message count). */
    return luat_status_from_liot(liot_rtos_queue_create(queue_handle, msg_size, msg_count));
}

int luat_rtos_queue_send(luat_rtos_queue_t queue_handle, void *data, uint32_t msg_size, uint32_t timeout)
{
    uint32_t actual_size = luat_rtos_normalize_msg_size(msg_size);

    if (queue_handle == NULL || data == NULL)
    {
        return -1;
    }

    if (luat_rtos_is_in_isr())
    {
        return luat_status_from_liot(liot_rtos_queue_release_isr(queue_handle, actual_size, (uint8_t *)data));
    }

    return luat_status_from_liot(liot_rtos_queue_release(queue_handle, actual_size, (uint8_t *)data, timeout));
}

int luat_rtos_queue_recv(luat_rtos_queue_t queue_handle, void *data, uint32_t *msg_size, uint32_t timeout)
{
    uint32_t actual_size = (msg_size == NULL) ? luat_rtos_normalize_msg_size(0U) : luat_rtos_normalize_msg_size(*msg_size);

    if (queue_handle == NULL || data == NULL)
    {
        return -1;
    }

    if (luat_rtos_is_in_isr())
    {
        return -1;
    }

    int status = luat_status_from_liot(liot_rtos_queue_wait(queue_handle, (uint8_t *)data, actual_size, timeout));

    if (msg_size != NULL)
    {
        *msg_size = actual_size;
    }
    return status;
}

int luat_rtos_queue_delete(luat_rtos_queue_t queue_handle)
{
    return luat_status_from_liot(liot_rtos_queue_delete(queue_handle));
}

int luat_rtos_queue_get_cnt(luat_rtos_queue_t queue_handle, uint32_t *cnt)
{
    if (queue_handle == NULL || cnt == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_queue_get_cnt(queue_handle, cnt));
}

int luat_rtos_semaphore_create(luat_rtos_semaphore_t *semaphore_handle, uint32_t initial_count)
{
    if (semaphore_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_semaphore_create(semaphore_handle, initial_count));
}

int luat_rtos_semaphore_delete(luat_rtos_semaphore_t semaphore_handle)
{
    if (semaphore_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_semaphore_delete(semaphore_handle));
}

int luat_rtos_semaphore_take(luat_rtos_semaphore_t semaphore_handle, uint32_t timeout)
{
    if (semaphore_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_semaphore_wait(semaphore_handle, timeout));
}

int luat_rtos_semaphore_release(luat_rtos_semaphore_t semaphore_handle)
{
    if (semaphore_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_semaphore_release(semaphore_handle));
}

int luat_rtos_flag_create(luat_rtos_flag_t *flag_handle)
{
    if (flag_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_flag_create(flag_handle));
}

int luat_rtos_flag_wait(luat_rtos_flag_t flag_handle,
                        uint32_t mask,
                        LUAT_FLAG_OP_E operation,
                        uint32_t *flags,
                        uint32_t timeout)
{
    if (flag_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_flag_wait(flag_handle, mask, operation, flags, timeout));
}

int luat_rtos_flag_release(luat_rtos_flag_t flag_handle, uint32_t mask, LUAT_FLAG_OP_E operation)
{
    if (flag_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_flag_release(flag_handle, mask, operation));
}

int luat_rtos_flag_delete(luat_rtos_flag_t flag_handle)
{
    if (flag_handle == NULL)
    {
        return -1;
    }

    return luat_status_from_liot(liot_rtos_flag_delete(flag_handle));
}

int luat_rtos_timer_create(luat_rtos_timer_t *timer_handle)
{
    luat_rtos_timer_ctx_t *ctx;

    if (timer_handle == NULL)
    {
        return -1;
    }

    if (*timer_handle != NULL)
    {
        return 0;
    }

    ctx = luat_rtos_timer_ctx_alloc();
    if (ctx == NULL)
    {
        return -1;
    }

    if (luat_status_from_liot(liot_rtos_timer_create(&ctx->timer,
                                                     LIOT_TimerOnce,
                                                     luat_rtos_timer_dispatch,
                                                     ctx)) != 0)
    {
        luat_rtos_timer_ctx_free(ctx);
        return -1;
    }

    *timer_handle = ctx->timer;
    return 0;
}

int luat_rtos_timer_start(luat_rtos_timer_t timer_handle,
                          uint32_t timeout,
                          uint8_t repeat,
                          luat_rtos_timer_callback_t callback_fun,
                          void *user_param)
{
    luat_rtos_timer_ctx_t *ctx = luat_rtos_timer_ctx_find(timer_handle);

    if (timer_handle == NULL || ctx == NULL)
    {
        return -1;
    }

    ctx->repeat = repeat ? 1U : 0U;
    ctx->timeout = timeout;
    ctx->callback = callback_fun;
    ctx->user_param = user_param;
    ctx->active = 1U;

    if (luat_rtos_is_in_isr())
    {
        return luat_status_from_liot(liot_rtos_timer_start_isr(timer_handle, timeout));
    }

    return luat_status_from_liot(liot_rtos_timer_start(timer_handle, timeout));
}

int luat_rtos_timer_stop(luat_rtos_timer_t timer_handle)
{
    luat_rtos_timer_ctx_t *ctx = luat_rtos_timer_ctx_find(timer_handle);

    if (timer_handle == NULL || ctx == NULL)
    {
        return -1;
    }

    ctx->active = 0U;

    if (luat_rtos_is_in_isr())
    {
        return luat_status_from_liot(liot_rtos_timer_stop_isr(timer_handle));
    }

    return luat_status_from_liot(liot_rtos_timer_stop(timer_handle));
}

int luat_rtos_timer_is_active(luat_rtos_timer_t timer_handle)
{
    luat_rtos_timer_ctx_t *ctx = luat_rtos_timer_ctx_find(timer_handle);

    if (timer_handle == NULL || ctx == NULL)
    {
        return 0;
    }

    if (luat_status_from_liot(liot_rtos_timer_is_running(timer_handle)) != 0)
    {
        ctx->active = 0U;
        return 0;
    }

    return ctx->active;
}

int luat_rtos_timer_delete(luat_rtos_timer_t timer_handle)
{
    luat_rtos_timer_ctx_t *ctx = luat_rtos_timer_ctx_find(timer_handle);
    int status;

    if (timer_handle == NULL || ctx == NULL)
    {
        return -1;
    }

    status = luat_status_from_liot(liot_rtos_timer_delete(timer_handle));
    if (status != 0)
    {
        return status;
    }

    luat_rtos_timer_ctx_free(ctx);
    return 0;
}

int luat_start_rtos_timer(luat_rtos_timer_t timer_handle,
                          uint32_t timeout,
                          uint8_t repeat,
                          luat_rtos_timer_callback_t callback_fun,
                          void *user_param)
{
    return luat_rtos_timer_start(timer_handle, timeout, repeat, callback_fun, user_param);
}

int luat_stop_rtos_timer(luat_rtos_timer_t timer_handle)
{
    return luat_rtos_timer_stop(timer_handle);
}

int luat_rtos_event_send(luat_rtos_task_handle task_handle,
                         uint32_t id,
                         uint32_t param1,
                         uint32_t param2,
                         uint32_t param3,
                         uint32_t timeout)
{
    luat_rtos_mailbox_t *mb;
    luat_event_t event;

    if (task_handle == NULL)
    {
        return -1;
    }

    mb = luat_rtos_mailbox_get(task_handle, 1);
    if (luat_rtos_mailbox_ensure_event_queue(mb) != 0)
    {
        return -1;
    }

    event.event_id = id;
    event.param1 = param1;
    event.param2 = param2;
    event.param3 = param3;

    if (luat_rtos_is_in_isr())
    {
        return luat_status_from_liot(liot_rtos_queue_release_isr(mb->event_queue,
                                                                 sizeof(event),
                                                                 (uint8_t *)&event));
    }

    return luat_status_from_liot(liot_rtos_queue_release(mb->event_queue,
                                                         sizeof(event),
                                                         (uint8_t *)&event,
                                                         timeout));
}

int luat_rtos_event_recv(luat_rtos_task_handle task_handle,
                         uint32_t wait_event_id,
                         luat_event_t *out_event,
                         luat_rtos_event_wait_callback_t *callback_fun,
                         uint32_t timeout)
{
    luat_rtos_mailbox_t *mb;
    luat_event_t event;
    uint32_t remain;
    uint32_t step;

    (void)callback_fun;

    if (out_event == NULL)
    {
        return -1;
    }

    if (task_handle == NULL)
    {
        task_handle = luat_rtos_get_current_handle();
    }

    mb = luat_rtos_mailbox_get(task_handle, 1);
    if (luat_rtos_mailbox_ensure_event_queue(mb) != 0)
    {
        return -1;
    }

    if (timeout == LUAT_WAIT_FOREVER)
    {
        for (;;)
        {
            if (luat_status_from_liot(liot_rtos_queue_wait(mb->event_queue,
                                                           (uint8_t *)&event,
                                                           sizeof(event),
                                                           LUAT_WAIT_FOREVER)) != 0)
            {
                return -1;
            }

            if (wait_event_id == 0U || event.event_id == wait_event_id)
            {
                *out_event = event;
                return 0;
            }
        }
    }

    remain = timeout;
    while (1)
    {
        step = (remain > LUAT_RTOS_POLL_SLICE_MS) ? LUAT_RTOS_POLL_SLICE_MS : remain;

        if (luat_status_from_liot(liot_rtos_queue_wait(mb->event_queue,
                                                       (uint8_t *)&event,
                                                       sizeof(event),
                                                       step)) == 0)
        {
            if (wait_event_id == 0U || event.event_id == wait_event_id)
            {
                *out_event = event;
                return 0;
            }
        }

        if (remain == 0U)
        {
            return -1;
        }

        if (remain <= step)
        {
            remain = 0U;
        }
        else
        {
            remain -= step;
        }
    }
}

int luat_rtos_message_send(luat_rtos_task_handle task_handle, uint32_t message_id, void *p_message)
{
    luat_rtos_mailbox_t *mb;
    luat_rtos_msg_t msg;

    if (task_handle == NULL)
    {
        return -1;
    }

    mb = luat_rtos_mailbox_get(task_handle, 1);
    if (luat_rtos_mailbox_ensure_msg_queue(mb) != 0)
    {
        return -1;
    }

    msg.task_handle = task_handle;
    msg.message_id = message_id;
    msg.message_ptr = p_message;

    if (luat_rtos_is_in_isr())
    {
        return luat_status_from_liot(liot_rtos_queue_release_isr(mb->msg_queue,
                                                                 sizeof(msg),
                                                                 (uint8_t *)&msg));
    }

    return luat_status_from_liot(liot_rtos_queue_release(mb->msg_queue,
                                                         sizeof(msg),
                                                         (uint8_t *)&msg,
                                                         LUAT_NO_WAIT));
}

int luat_rtos_message_recv(luat_rtos_task_handle task_handle,
                           uint32_t *message_id,
                           void **p_p_message,
                           uint32_t timeout)
{
    luat_rtos_mailbox_t *mb;
    luat_rtos_msg_t msg;

    if (task_handle == NULL)
    {
        task_handle = luat_rtos_get_current_handle();
    }

    mb = luat_rtos_mailbox_get(task_handle, 1);
    if (luat_rtos_mailbox_ensure_msg_queue(mb) != 0)
    {
        return -1;
    }

    if (luat_status_from_liot(liot_rtos_queue_wait(mb->msg_queue,
                                                   (uint8_t *)&msg,
                                                   sizeof(msg),
                                                   timeout)) != 0)
    {
        return -1;
    }

    if (message_id != NULL)
    {
        *message_id = msg.message_id;
    }

    if (p_p_message != NULL)
    {
        *p_p_message = msg.message_ptr;
    }

    return 0;
}

uint32_t luat_rtos_entry_critical(void)
{
    if (luat_rtos_is_in_isr())
    {
        return liot_rtos_enter_critical_from_isr();
    }

    liot_rtos_enter_critical();
    return 0U;
}

void luat_rtos_exit_critical(uint32_t critical_token)
{
    if (luat_rtos_is_in_isr())
    {
        liot_rtos_exit_critical_from_isr(critical_token);
        return;
    }

    (void)critical_token;
    liot_rtos_exit_critical();
}