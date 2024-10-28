
#ifndef _THREAD_FUNCTIONS_H_
#define _THREAD_FUNCTIONS_H_

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

typedef struct 
{
    SemaphoreHandle_t a;
    SemaphoreHandle_t b;
    char *thread_name;
} semaphores_t;

void thread_lock(void *params);

void big_increment_print(SemaphoreHandle_t semaphore, int* counter);

void increment_print(SemaphoreHandle_t semaphore, int* counter);

#endif
