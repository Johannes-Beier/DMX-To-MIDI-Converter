
#include <stdint.h>
#include <stdio.h>
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/projdefs.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"

#include "main.h"
void midi_init(void)
{
    //Konfigurationsstruktur anlegen
    const uart_config_t uart_config = 
    {
        .baud_rate  = MIDI_UART,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    //Uart-Treiber einrichten
    ESP_ERROR_CHECK
    (
        uart_driver_install(MIDI_UART, 0, 0, 0, NULL, 0)
    );
    //Konfiguration auf Uart2 anwenden
    ESP_ERROR_CHECK
    (
        uart_param_config(MIDI_UART, &uart_config)
    );
    // Pin zu UART zuweisen
    ESP_ERROR_CHECK
    (
        uart_set_pin(MIDI_UART, MIDI_TX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)
    );
}

void midi_send(uint8_t status, uint8_t data1, uint8_t data2)
{
    uint8_t msg[3] = {status, data1, data2};
    ESP_ERROR_CHEK
    (
        uart_write_bytes(MIDI_UART, (const char *) msg, sizeof(msg))
    );
}

void app_main(void)
{
    midi_init();
    //Hauptprogramm
    while(1)
    {
        midi_send(0x90, 60, 100);   //Senden
        vTaskDelay(pdMS_TO_TICKS(500));   //Warten
        midi_send(0x80, 60, 0);     //Senden
        vTaskDelay(pdMS_TO_TICKS(500));   //Warten
    }
}
