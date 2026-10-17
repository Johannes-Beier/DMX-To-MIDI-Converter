/**
* @file dmx.c
* @brief Funktionen für den DMX-Empfang über UART
*/



#include "driver/uart.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "main.h"
#include "portmacro.h"
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/queue.h"
#include <stdbool.h>

#ifndef DMX_SKIP_BREAK_BYTES
#define DMX_SKIP_BREAK_BYTES 0
#endif


//Veröffentlichte Kanalwerte
static uint8_t s_channels[512];
static uint32_t s_frame_count;
static int64_t s_last_frame_us;
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;

//Arbeitspuffer: Startcode +512 Kanäle
static uint8_t s_rx[512 + 1];
static size_t s_rx_len;
static bool s_synced;
static size_t s_skip;



static void append_rx_data(void)
{
    uint8_t tmp[128];
    int32_t n = uart_read_bytes(DMX_MODUL, tmp, sizeof(tmp), 0U);
    while(n > 0)
    {
        if(!s_synced) 
        {
            continue;
        }
        for(size_t i = 0U; i < (size_t)n; i++)
        {
            if(s_skip > 0)
            {
                s_skip--;
                continue;
            }
            if(s_rx_len < sizeof(s_rx)) {
                s_rx[s_rx_len] = tmp[i];
                s_rx_len++;
            }
        }
        n = uart_read_bytes(DMX_MODUL, tmp, sizeof(tmp), 0U);
    }
}

static void commit_frame(void)
{
    if(!s_synced || s_rx_len < 2U || s_rx[0] != 0x00U)
    {
        return;
    }
    size_t n = s_rx_len - 1;
    portENTER_CRITICAL(&s_lock);
    memcpy(s_channels, &s_rx[1], n);
    s_frame_count++;
    s_last_frame_us = esp_timer_get_time();
    portEXIT_CRITICAL(&s_lock);
}

void dmx_task(void *arg)
{
    uart_event_t ev;

    while(1)
    {
        if(xQueueReceive(receive_queue, &ev, portMAX_DELAY) != pdTRUE)
        {
            continue;
        }
        switch(ev.type) 
        {
            case UART_DATA:
                append_rx_data();
                break;
            case UART_BREAK:
                append_rx_data();
                commit_frame();
                s_rx_len = 0;
                s_skip = DMX_SKIP_BREAK_BYTES;
                s_synced = true;
                break;
            case UART_FIFO_OVF:
            case UART_BUFFER_FULL:
                ESP_LOGW("DMX", "Überlauf resynchronisiere");
                (void)uart_flush_input(DMX_MODUL);
                xQueueReset(receive_queue);
                s_rx_len = 0;
                s_synced = false;
                break;
                default:
            break;
        }
    }
}

bool dmx_receiver_get(uint8_t *zielVar, uint16_t first_channel, uint16_t count)
{
    if((zielVar == NULL) || (first_channel < 1) || (count == 0) || (((uint32_t)first_channel -1U + count) > 512U))
    {
        return false;
    }
    portENTER_CRITICAL(&s_lock);
    memcpy(zielVar, &s_channels[first_channel -1], count);
    portEXIT_CRITICAL(&s_lock);
    return true;
}

bool dmx_receiver_is_active(uint32_t timeout_ms)
{
    portENTER_CRITICAL(&s_lock);
    int64_t last = s_last_frame_us;
    uint32_t count = s_frame_count;
    portEXIT_CRITICAL(&s_lock);
    uint64_t now = esp_timer_get_time();
    return (count > 0) && ((now - last) < ((int64_t)timeout_ms * 1000));
}
