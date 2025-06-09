#include "osal_wrapper.h"

void thread_func1(void* parameter)
{
    for (int i = 0; i < 10; ++i)
    {
        osal_printf("Thread 1: log %d\n", i);
        osal_sleep_ms(200);
    }
    osal_printf("Thread 1 exit\n");
}

void thread_func2(void* parameter)
{
    for (int i = 0; i < 10; ++i)
    {
        osal_printf("Thread 2: log %d\n", i);
        osal_sleep_ms(100);
    }
    osal_printf("Thread 2 exit\n");
}

int main_thread_demo(void)
{
    osal_thread_t t1 = osal_thread_create("t1", thread_func1, NULL, 1024, 10, 10);
    osal_thread_t t2 = osal_thread_create("t2", thread_func2, NULL, 1024, 10, 10);

    osal_printf("===================== Thread Demo =====================\n");

    osal_thread_start(t1);
    osal_thread_start(t2);

    osal_sleep_ms(3000);

    osal_thread_delete(t1);
    osal_thread_delete(t2);

    return 0;
}

static osal_sem_t sem;

void thread_func3(void* parameter)
{
    for (int i = 0; i < 5; ++i)
    {
        osal_printf("Thread 3: log %d\n", i);
        osal_sem_release(sem);
        osal_sleep_ms(200);
    }
    osal_printf("Thread 3 exit\n");
}

void thread_func4(void* parameter)
{
    for (int i = 0; i < 5; ++i)
    {
        osal_sem_take(sem, OSAL_WAIT_FOREVER);
        osal_printf("Thread 4: log %d\n", i);
    }
    osal_printf("Thread 4 exit\n");
}

int main_sem_demo(void)
{
    sem = osal_sem_create("sem", 0);
    osal_thread_t t1 = osal_thread_create("t1", thread_func3, NULL, 1024, 10, 10);
    osal_thread_t t2 = osal_thread_create("t2", thread_func4, NULL, 1024, 10, 10);

    osal_printf("===================== Sem Demo =====================\n");

    osal_thread_start(t1);
    osal_thread_start(t2);

    osal_sleep_ms(3000);

    osal_thread_delete(t1);
    osal_thread_delete(t2);

    return 0;
}

static osal_mutex_t mutex;

void thread_func5(void* parameter)
{
    osal_sleep_ms(100);
    osal_printf("Thread 5 waiting for mutex\n");
    osal_mutex_take(mutex, OSAL_WAIT_FOREVER);
    osal_printf("Thread 5 acquired mutex\n");
}

void thread_func6(void* parameter)
{
    osal_mutex_take(mutex, OSAL_WAIT_FOREVER);
    osal_sleep_ms(2000);
    osal_printf("Thread 6 release mutex\n");
    osal_mutex_release(mutex);
}

int main_mutex_demo(void)
{
    mutex = osal_mutex_create("mutex");
    osal_thread_t t1 = osal_thread_create("t1", thread_func5, NULL, 1024, 10, 10);
    osal_thread_t t2 = osal_thread_create("t2", thread_func6, NULL, 1024, 10, 10);

    osal_printf("===================== Mutex Demo =====================\n");

    osal_thread_start(t1);
    osal_thread_start(t2);

    osal_sleep_ms(3000);

    osal_thread_delete(t1);
    osal_thread_delete(t2);
}

osal_mq_t mq;

void thread_func7(void* parameter)
{
    char *msg;
    osal_sleep_ms(100);
    osal_printf("Thread 5 waiting for message\n");
    osal_mq_recv(mq, (void *)&msg, sizeof(char *), OSAL_WAIT_FOREVER);
    osal_printf("Thread 5 receive message: %s\n", msg);
    osal_free(msg);
}

void thread_func8(void* parameter)
{
    char *msg = NULL;
    msg = (char *)osal_malloc(32);
    if (msg == NULL) {
        osal_printf("Thread 6 malloc failed\n");
        return;
    }
    snprintf(msg, 32, "Hello from Thread 6");
    osal_sleep_ms(2000);
    osal_printf("Thread 6 send message\n");
    osal_mq_send(mq, (const void *)&msg, sizeof(char *), OSAL_WAIT_FOREVER);
}

int main_mq_demo(void)
{
    mq = osal_mq_create("mq", sizeof(char *), 1);
    osal_thread_t t1 = osal_thread_create("t1", thread_func7, NULL, 1024, 10, 10);
    osal_thread_t t2 = osal_thread_create("t2", thread_func8, NULL, 1024, 10, 10);

    osal_printf("===================== Mq Demo =====================\n");

    osal_thread_start(t1);
    osal_thread_start(t2);

    osal_sleep_ms(3000);

    osal_thread_delete(t1);
    osal_thread_delete(t2);
}


int main()
{
    osal_printf("OSAL Demostrate!\n");
    // main_thread_demo();
    // main_sem_demo();
    // main_mutex_demo();
    main_mq_demo();
    return 0;
}