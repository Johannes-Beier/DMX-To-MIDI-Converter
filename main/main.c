
#include <stdint.h>
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/uart_types.h"


#include "main.h"

/**
*   Hauptfunktion
*/
void app_main(void)
{
    /* Aufruf der Initialisierungen */
    dmx_init();
    midi_init();
    xTaskCreate(dmx_task, "dmx_rx", 4096, NULL, configMAX_PRIORITIES -3, NULL);
    //Hauptprogramm
    while(1)
    {
        midi_send(0x90, 60, 100);   //Senden
        vTaskDelay(pdMS_TO_TICKS(500));   //Warten
        midi_send(0x80, 60, 0);     //Senden
        vTaskDelay(pdMS_TO_TICKS(500));   //Warten
    }
}

void midi_send(uint8_t status, uint8_t data1, uint8_t data2)
{
    uint8_t msg[3] = {status, data1, data2};
    ESP_ERROR_CHECK
    (
        uart_write_bytes(UART_NUM_2, (const char *) msg, sizeof(msg))
    );
}