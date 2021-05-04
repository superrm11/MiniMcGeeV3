#ifndef _COMS_
#define _COMS_

#include "esp_wifi.h"
#include "mdns.h"
#include "lwip/sockets.h"

// Buffer to receive info from over the network
typedef struct 
{
    int32_t leftx;
    int32_t lefty;
    int32_t rightx;
} sock_buffer_t;

volatile sock_buffer_t* curr_data;

TaskHandle_t* socket_task_handle;

SemaphoreHandle_t socket_mux;

void coms_install();

void socket_task(void* ptr);

#endif