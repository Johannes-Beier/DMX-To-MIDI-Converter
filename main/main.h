#pragma once
// Port konfigurieren
#define DMX_PORT 17
#define DMX_MODUL UART_NUM_1
#define MIDI_PORT 4
#define MIDI_MODUL UART_NUM_2

#include <stdint.h>

/**
*   Initialisiert die UART-2-Schnittschtelle zur Ausgabe des MIDI-Signals
*/
void midi_init(void);

/**
*   Initialisiert die UART-1-Schnittschtelle zur Einlese des DMX-Signals
*/
void dmx_init(void);

/**
* Gute Frage, was das macht
*/
void dmx_task(void *arg);
/**
* Macht auch irgendwas
*/
bool dmx_receiver_get(uint8_t *dst, uint16_t first_channel, uint16_t count);
/**
* Existiert halt auch
*/
bool dmx_receiver_is_active(uint32_t timeout_ms);
/**
* Wird später hoffentlich mal benötigt
*/
void midi_send(uint8_t status, uint8_t data1, uint8_t data2);
