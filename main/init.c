#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "hal/uart_types.h"
#include "esp_log.h"

 QueueHandle_t receive_queue;

/**
*   Initialisiert die UART-2-Schnittschtelle zur Ausgabe des MIDI-Signals
*/
void midi_init(void)
{
    /* Signalstruktur für 8N1-Aufbau anlegen */
    const uart_config_t uart_config = 
    {
        .baud_rate  = 31250,                    /* Baudrate */
        .data_bits  = UART_DATA_8_BITS,         /* 8-Bit pro Zeichen */
        .parity     = UART_PARITY_DISABLE,      /* Kein Paritätsbit */
        .stop_bits  = UART_STOP_BITS_1,         /* Ein Stopbit */
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE, /* Keine Sendemengensteuerung */
        .source_clk = UART_SCLK_DEFAULT,        /* Standardclock*/
    };

    /* UART-Treiber initialisieren */
    ESP_ERROR_CHECK
    (
        uart_driver_install
        (
            UART_NUM_2,                         /* UART-2 benutzen */
            256,                                /* Kein Empfangspuffer nötig, muss aber gesetzt sein*/
            0,                                  /* Kein Sendepuffer */
            0,                                  /* Keine Event-Queue*/
            NULL,                               /* Entsprechend auch kein Handle für Queue*/
            0                                   /* Keine Interruptzuweisung*/
        )
    );

    /* Signalstruktur auf UART-2 anwenden */
    ESP_ERROR_CHECK
    (
        uart_param_config(UART_NUM_2, &uart_config)
    );

    /* Pins zu UART zuweisen */
    ESP_ERROR_CHECK
    (
        uart_set_pin
        (
            UART_NUM_2,                  /* UART-2 benutzen */
            4,                         /* Auf Pin 4 senden*/
            UART_PIN_NO_CHANGE,         /* Auf keinem Pin empfangen */
            UART_PIN_NO_CHANGE,        /* Keine Pins für Sendemengensteuerung */
            UART_PIN_NO_CHANGE         /* Keine Pins für Sendemengensteuerung */
        )
    );
    
    ESP_LOGI("UART-2", "als MIDI-Sender initialisiert");
}


/**
*   Initialisiert die UART-1-Schnittschtelle zur Einlese des DMX-Signals
*/
void dmx_init(void)
{
    /* Signalstruktur für 8N2-Aufbau anlegen */
    const uart_config_t uart_config = 
    {
        .baud_rate  = 250000,                       /* Baudrate */
        .data_bits  = UART_DATA_8_BITS,             /* 8-Bit pro Zeichen */
        .parity     = UART_PARITY_DISABLE,          /* Kein Paritätsbit */
        .stop_bits  = UART_STOP_BITS_2,             /* Zwei Stopbits */
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,     /* Keine Sendemengensteuerung */
        .source_clk = UART_SCLK_DEFAULT,            /* Standardclock*/
    };

    /* UART-Treiber initialisieren */
    ESP_ERROR_CHECK
    (
        uart_driver_install
        (
            UART_NUM_1,                             /* UART-1 benutzen */
            1024,                                   /* 1024 Bytes Empfangspuffer */
            0,                                      /* Kein Sendepuffer */
            20,                                     /* max 20 Events in Queue */
            &receive_queue,                         /* Handle für Event-Queue */
        0                                           /* Keine Interruptzuweisung */
        )
    );

    /* Signalstruktur auf UART-1 anwenden */
    ESP_ERROR_CHECK
    (
        uart_param_config(UART_NUM_1, &uart_config)
    );

    /* Pins zu UART zuweisen */
    ESP_ERROR_CHECK
    (
        uart_set_pin
        (
            UART_NUM_1,                     /* UART-2 benutzen */
            UART_PIN_NO_CHANGE,             /* Auf keinem Pin senden */
            17,                            /* Auf Pin 17 empfangen */
            UART_PIN_NO_CHANGE,           /* Keine Pins für Sendemengensteuerung */
            UART_PIN_NO_CHANGE            /* Keine Pins für Sendemengensteuerung */
            )
    );
    ESP_ERROR_CHECK
    (
        uart_set_rx_timeout(UART_NUM_1, 1)
    );
    ESP_LOGI("UART-1", "als DMX-Empfaenger initialisiert");
}
