#ifndef OSAL_WRAPPER_H
#define OSAL_WRAPPER_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OSAL_WAIT_FOREVER   (0xFFFFFFFF)
#define OSAL_WAIT_NO        (0)

// 线程
typedef void* osal_thread_t;
typedef void (*osal_thread_entry_t)(void* parameter);

osal_thread_t osal_thread_create(const char* name, osal_thread_entry_t entry, void* parameter, uint32_t stack_size, uint8_t priority, uint32_t tick);
int osal_thread_start(osal_thread_t thread);
int osal_thread_delete(osal_thread_t thread);

// 信号量
typedef void* osal_sem_t;

osal_sem_t osal_sem_create(const char* name, uint32_t value);
int osal_sem_take(osal_sem_t sem, uint32_t timeout);
int osal_sem_release(osal_sem_t sem);
int osal_sem_delete(osal_sem_t sem);

// 互斥量
typedef void* osal_mutex_t;

osal_mutex_t osal_mutex_create(const char* name);
int osal_mutex_take(osal_mutex_t mutex, uint32_t timeout);
int osal_mutex_release(osal_mutex_t mutex);
int osal_mutex_delete(osal_mutex_t mutex);

// 消息队列
typedef void* osal_mq_t;

osal_mq_t osal_mq_create(const char* name, uint32_t msg_size, uint32_t max_msgs);
int osal_mq_send(osal_mq_t mq, const void* msg, uint32_t size, uint32_t timeout);
int osal_mq_recv(osal_mq_t mq, void* msg, uint32_t size, uint32_t timeout);
int osal_mq_delete(osal_mq_t mq);

// 邮箱
typedef void* osal_mb_t;

osal_mb_t osal_mb_create(const char* name, uint32_t size);
int osal_mb_send(osal_mb_t mb, uint32_t value, uint32_t timeout);
int osal_mb_recv(osal_mb_t mb, uint32_t* value, uint32_t timeout);
int osal_mb_delete(osal_mb_t mb);

int osal_printf(const char* fmt, ...);

int osal_sleep_ms(uint32_t ms);

void* osal_malloc(size_t size);
void  osal_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif // OSAL_WRAPPER_H