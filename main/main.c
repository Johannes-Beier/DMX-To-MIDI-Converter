
#include <stdint.h>
#include "driver/uart.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "main.h"

/**
*   Hauptfunktion
*/
void app_main(void)
{
    /* Aufruf der Initialisierungen */
    uint8_t dmx_wert[512] ={0u};
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    dmx_init();
    midi_init();
    xTaskCreate(dmx_task, "dmx_rx", 4096, NULL, configMAX_PRIORITIES -3, NULL);
    //Hauptprogramm
    while(1)
    {
        if(dmx_receiver_is_active(500)) {
            dmx_receiver_get(dmx_wert, 1, 512);
            if(dmx_wert[1] > 125)
            {
                gpio_set_level(GPIO_NUM_5, 1);
            } else{
                gpio_set_level(GPIO_NUM_5, 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));   //Warten
    }
}

void midi_send(uint8_t status, uint8_t data1, uint8_t data2)
{
    uint8_t msg[3] = {status, data1, data2};
    
    int geschrieben= uart_write_bytes(MIDI_MODUL, (const char *) msg, sizeof(msg));
    if (geschrieben != (int)sizeof(msg))
    {
        ESP_LOGW("MIDI", "Senden fehlgeschlagen");
    }
}