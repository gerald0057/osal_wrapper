#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "osal_wrapper.h"

// ===================== 线程实现略 =====================

typedef struct
{
    pthread_t thread;
    osal_thread_entry_t entry;
    void* parameter;
    char name[32];
    sem_t start_sem; // 用于控制启动
} osal_linux_thread_t;

static void* osal_linux_thread_entry(void* arg)
{
    osal_linux_thread_t* t = (osal_linux_thread_t*)arg;
    // 等待 start 信号
    sem_wait(&t->start_sem);
    if (t && t->entry)
        t->entry(t->parameter);
    return NULL;
}

osal_thread_t osal_thread_create(const char* name, osal_thread_entry_t entry, void* parameter, uint32_t stack_size, uint8_t priority, uint32_t tick)
{
    osal_linux_thread_t* t = (osal_linux_thread_t*)malloc(sizeof(osal_linux_thread_t));
    if (!t) return NULL;
    memset(t, 0, sizeof(*t));
    t->entry = entry;
    t->parameter = parameter;
    if (name)
        strncpy(t->name, name, sizeof(t->name) - 1);

    sem_init(&t->start_sem, 0, 0); // 初始化为0，阻塞

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (stack_size > 0)
        pthread_attr_setstacksize(&attr, stack_size);

    int ret = pthread_create(&t->thread, &attr, osal_linux_thread_entry, t);
    pthread_attr_destroy(&attr);
    if (ret != 0)
    {
        sem_destroy(&t->start_sem);
        free(t);
        return NULL;
    }
    return (osal_thread_t)t;
}

int osal_thread_delete(osal_thread_t thread)
{
    osal_linux_thread_t* t = (osal_linux_thread_t*)thread;
    if (!t) return -1;
    pthread_cancel(t->thread);
    pthread_join(t->thread, NULL);
    free(t);
    return 0;
}

int osal_thread_start(osal_thread_t thread)
{
    osal_linux_thread_t* t = (osal_linux_thread_t*)thread;
    if (!t) return -1;
    return sem_post(&t->start_sem);
}

// ===================== 信号量 =====================
typedef struct {
    sem_t sem;
} osal_linux_sem_t;

osal_sem_t osal_sem_create(const char* name, uint32_t value) {
    osal_linux_sem_t* s = (osal_linux_sem_t*)malloc(sizeof(osal_linux_sem_t));
    if (!s) return NULL;
    sem_init(&s->sem, 0, value);
    return (osal_sem_t)s;
}

int osal_sem_take(osal_sem_t sem, uint32_t timeout) {
    osal_linux_sem_t* s = (osal_linux_sem_t*)sem;
    if (!s) return -1;
    if (timeout == OSAL_WAIT_NO) {
        // 立即返回，不等待
        return sem_trywait(&s->sem);
    } else if (timeout == OSAL_WAIT_FOREVER) {
        // 无限等待
        return sem_wait(&s->sem);
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }
        return sem_timedwait(&s->sem, &ts);
    }
}

int osal_sem_release(osal_sem_t sem) {
    osal_linux_sem_t* s = (osal_linux_sem_t*)sem;
    if (!s) return -1;
    return sem_post(&s->sem);
}

int osal_sem_delete(osal_sem_t sem) {
    osal_linux_sem_t* s = (osal_linux_sem_t*)sem;
    if (!s) return -1;
    sem_destroy(&s->sem);
    free(s);
    return 0;
}

// ===================== 互斥量 =====================
typedef struct {
    pthread_mutex_t mutex;
} osal_linux_mutex_t;

osal_mutex_t osal_mutex_create(const char* name) {
    osal_linux_mutex_t* m = (osal_linux_mutex_t*)malloc(sizeof(osal_linux_mutex_t));
    if (!m) return NULL;
    pthread_mutex_init(&m->mutex, NULL);
    return (osal_mutex_t)m;
}

int osal_mutex_take(osal_mutex_t mutex, uint32_t timeout) {
    osal_linux_mutex_t* m = (osal_linux_mutex_t*)mutex;
    if (!m) return -1;
    if (timeout == 0xFFFFFFFF)
        return pthread_mutex_lock(&m->mutex);
    else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }
        return pthread_mutex_timedlock(&m->mutex, &ts);
    }
}

int osal_mutex_release(osal_mutex_t mutex) {
    osal_linux_mutex_t* m = (osal_linux_mutex_t*)mutex;
    if (!m) return -1;
    return pthread_mutex_unlock(&m->mutex);
}

int osal_mutex_delete(osal_mutex_t mutex) {
    osal_linux_mutex_t* m = (osal_linux_mutex_t*)mutex;
    if (!m) return -1;
    pthread_mutex_destroy(&m->mutex);
    free(m);
    return 0;
}

// ===================== 消息队列 =====================
// 简单实现：循环队列+互斥量+信号量
typedef struct {
    void* buffer;
    uint32_t msg_size;
    uint32_t max_msgs;
    uint32_t head;
    uint32_t tail;
    sem_t sem_used;
    sem_t sem_free;
    pthread_mutex_t mutex;
} osal_linux_mq_t;

osal_mq_t osal_mq_create(const char* name, uint32_t msg_size, uint32_t max_msgs) {
    osal_linux_mq_t* mq = (osal_linux_mq_t*)malloc(sizeof(osal_linux_mq_t));
    if (!mq) return NULL;
    mq->buffer = malloc(msg_size * max_msgs);
    if (!mq->buffer) { free(mq); return NULL; }
    mq->msg_size = msg_size;
    mq->max_msgs = max_msgs;
    mq->head = mq->tail = 0;
    sem_init(&mq->sem_used, 0, 0);
    sem_init(&mq->sem_free, 0, max_msgs);
    pthread_mutex_init(&mq->mutex, NULL);
    return (osal_mq_t)mq;
}

int osal_mq_send(osal_mq_t mq_, const void* msg, uint32_t size, uint32_t timeout) {
    osal_linux_mq_t* mq = (osal_linux_mq_t*)mq_;
    if (!mq || size != mq->msg_size) return -1;
    if (osal_sem_take(&mq->sem_free, timeout) != 0) return -1;
    pthread_mutex_lock(&mq->mutex);
    memcpy((char*)mq->buffer + mq->tail * mq->msg_size, msg, mq->msg_size);
    mq->tail = (mq->tail + 1) % mq->max_msgs;
    pthread_mutex_unlock(&mq->mutex);
    sem_post(&mq->sem_used);
    return 0;
}

int osal_mq_recv(osal_mq_t mq_, void* msg, uint32_t size, uint32_t timeout) {
    osal_linux_mq_t* mq = (osal_linux_mq_t*)mq_;
    if (!mq || size != mq->msg_size) return -1;
    if (osal_sem_take(&mq->sem_used, timeout) != 0) return -1;
    pthread_mutex_lock(&mq->mutex);
    memcpy(msg, (char*)mq->buffer + mq->head * mq->msg_size, mq->msg_size);
    mq->head = (mq->head + 1) % mq->max_msgs;
    pthread_mutex_unlock(&mq->mutex);
    sem_post(&mq->sem_free);
    return 0;
}

int osal_mq_delete(osal_mq_t mq_) {
    osal_linux_mq_t* mq = (osal_linux_mq_t*)mq_;
    if (!mq) return -1;
    sem_destroy(&mq->sem_used);
    sem_destroy(&mq->sem_free);
    pthread_mutex_destroy(&mq->mutex);
    free(mq->buffer);
    free(mq);
    return 0;
}

// ===================== 邮箱 =====================
// 简单实现：循环队列+互斥量+信号量，邮箱内容为uint32_t
typedef struct {
    uint32_t* buffer;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    sem_t sem_used;
    sem_t sem_free;
    pthread_mutex_t mutex;
} osal_linux_mb_t;

osal_mb_t osal_mb_create(const char* name, uint32_t size) {
    osal_linux_mb_t* mb = (osal_linux_mb_t*)malloc(sizeof(osal_linux_mb_t));
    if (!mb) return NULL;
    mb->buffer = (uint32_t*)malloc(sizeof(uint32_t) * size);
    if (!mb->buffer) { free(mb); return NULL; }
    mb->size = size;
    mb->head = mb->tail = 0;
    sem_init(&mb->sem_used, 0, 0);
    sem_init(&mb->sem_free, 0, size);
    pthread_mutex_init(&mb->mutex, NULL);
    return (osal_mb_t)mb;
}

int osal_mb_send(osal_mb_t mb_, uint32_t value, uint32_t timeout) {
    osal_linux_mb_t* mb = (osal_linux_mb_t*)mb_;
    if (!mb) return -1;
    if (osal_sem_take(&mb->sem_free, timeout) != 0) return -1;
    pthread_mutex_lock(&mb->mutex);
    mb->buffer[mb->tail] = value;
    mb->tail = (mb->tail + 1) % mb->size;
    pthread_mutex_unlock(&mb->mutex);
    sem_post(&mb->sem_used);
    return 0;
}

int osal_mb_recv(osal_mb_t mb_, uint32_t* value, uint32_t timeout) {
    osal_linux_mb_t* mb = (osal_linux_mb_t*)mb_;
    if (!mb || !value) return -1;
    if (osal_sem_take(&mb->sem_used, timeout) != 0) return -1;
    pthread_mutex_lock(&mb->mutex);
    *value = mb->buffer[mb->head];
    mb->head = (mb->head + 1) % mb->size;
    pthread_mutex_unlock(&mb->mutex);
    sem_post(&mb->sem_free);
    return 0;
}

int osal_mb_delete(osal_mb_t mb_) {
    osal_linux_mb_t* mb = (osal_linux_mb_t*)mb_;
    if (!mb) return -1;
    sem_destroy(&mb->sem_used);
    sem_destroy(&mb->sem_free);
    pthread_mutex_destroy(&mb->mutex);
    free(mb->buffer);
    free(mb);
    return 0;
}

int osal_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}

int osal_sleep_ms(uint32_t ms)
{
    return usleep(ms * 1000);
}

void* osal_malloc(size_t size) {
    return malloc(size);
}

void osal_free(void* ptr) {
    free(ptr);
}
