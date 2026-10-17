/**
* @file main.h
* @brief Deklariert Schnittstellen für DMX-Empfang und MIDI-Ausgabe
*/


#ifndef MAIN_H
#define MAIN_H
// Port konfigurieren

#define DMX_PORT 17
#define DMX_MODUL UART_NUM_1
#define MIDI_PORT 4
#define MIDI_MODUL UART_NUM_2

#include "freertos/idf_additions.h"
#include <stdbool.h>
extern QueueHandle_t receive_queue;

#include <stdint.h>


/**
* @brief    Initialisiert die UART-2-Schnittschtelle zur Ausgabe des MIDI-Signals
*/
void midi_init(void);


/**
* @brief    Initialisiert die UART-1-Schnittschtelle zur Einlese des DMX-Signals
*/
void dmx_init(void);


/**
* @brief    Task für den DMX-Empfang
* 
* Ein eigenständiger Task für den DMX-Empfang, damit dieser unabhängig vom restlichen Programm läuft und immer ganze Frames auswertet.
*
* @param[in] arg    Nicht verwendet @c NULL übergeben. 
* @pre  dmx_init() muss aufgerufen werden. 
*/
void dmx_task(void *arg);


/**
* @brief    Kopiert die Werte des letzten empfangenen Frames.
*
* @param[out]   dst             Zieladresse des Arrays, in das gespeichert werden soll. (Muss mindestens @p count Bytes groß sein). 
* @param[in]    first_channel   Erster Kanal, der gespeichert werden soll.
* @param[in]    count           Anzahl der zu speichernden Kanäle.
* 
* @retval true  Werte wurden erfolgreich kopiert.
* @retval false Mindestens ein ungütliger Parameter, @p dst wurde nicht verändert.
*/
bool dmx_receiver_get(uint8_t *dst, uint16_t first_channel, uint16_t count);


/**
* @brief        Prüft auf einen kürzlich empfangenen DMX-Frame.
* 
* @param[in]    timeout_ms Maximalzeit, die der letzte empfangene Frame alt sein darf. 
*
* @retval true  Letzter Frame ist jünger als @p timeout_ms .
* @retval false Kein Frame im Zeitbereich empfangen.
*/
bool dmx_receiver_is_active(uint32_t timeout_ms);


/**
* @brief Sendet eine MIDI-Nachricht. 
*
* @param[in] status Note ein/aus und MIDI-Kanal.
* @param[in] data1  Note bzw. DMX-Kanal.
* @param[in] data2  Anschlagsstärke bzw. DMX-Wert.
*
* @pre midi_init() muss aufgerufen werden. 
*/
void midi_send(uint8_t status, uint8_t data1, uint8_t data2);

#endif /* MAIN_H*/