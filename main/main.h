
#include <stdint.h>

void midi_init(void);
void dmx_init(void);
void dmx_task(void *arg);

void midi_send(uint8_t, uint8_t, uint8_t);
