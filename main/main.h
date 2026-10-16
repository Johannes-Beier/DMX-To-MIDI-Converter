#pragma once
#include <stdint.h>

void midi_init(void);
void dmx_init(void);
void dmx_task(void *arg);
uint8_t dmx_receiver_get(uint8_t *dst, uint16_t first_channel, uint16_t count);
uint8_t dmx_receiver_is_active(uint32_t timeout_ms);
void midi_send(uint8_t, uint8_t, uint8_t);
