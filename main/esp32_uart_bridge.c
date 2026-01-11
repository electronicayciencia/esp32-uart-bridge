/*
 *        esp32_uart_bridge.c
 *
 *        Based on the work of (c) 2023 Alien Green LLC
 *
 */

#include <fcntl.h>
#include <stdio.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "usb_direct.h"


#define UBRIDGE_PIN_TXD         CONFIG_UBRIDGE_UART_TXD
#define UBRIDGE_PIN_RXD         CONFIG_UBRIDGE_UART_RXD
#define UBRIDGE_PIN_RTS         UART_PIN_NO_CHANGE
#define UBRIDGE_PIN_CTS         UART_PIN_NO_CHANGE
#define UBRIDGE_UART_PORT_NUM   CONFIG_UBRIDGE_UART_PORT_NUM
#define UBRIDGE_UART_BAUD_RATE  CONFIG_UBRIDGE_UART_BAUD_RATE

#define USB_READ_CHUNK_SIZE 64

void app_main(void) {
    
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
            .baud_rate = UBRIDGE_UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .source_clk = UART_SCLK_XTAL,
    };

    ESP_ERROR_CHECK(uart_driver_install(UBRIDGE_UART_PORT_NUM, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UBRIDGE_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UBRIDGE_UART_PORT_NUM, 
                                 UBRIDGE_PIN_TXD,
                                 UBRIDGE_PIN_RXD,
                                 UBRIDGE_PIN_RTS,
                                 UBRIDGE_PIN_CTS));

    ESP_LOGI("Bridge", "UART Port %d, GPIO: Rx %d, Tx %d", 
        UBRIDGE_UART_PORT_NUM, 
        UBRIDGE_PIN_RXD, 
        UBRIDGE_PIN_TXD);

    ESP_LOGI("Bridge", "UART Baudrate %d, Data bits 8, Parity None, Stop bits 1",
        UBRIDGE_UART_BAUD_RATE);


    /* Not using usb_serial_jtag_driver 
     * See https://github.com/electronicayciencia/esp32-misc/tree/master/usb_serial_jtag_read */

    /* Configure a bridge buffer for the i/o data */
    uint8_t buf[USB_READ_CHUNK_SIZE];

    while (true) {
        // USB->UART
        int rx = usb_read(buf, sizeof(buf));
        //ESP_LOGI("USB->UART", "Read %d bytes", rx);
        if (rx > 0) {
            uart_write_bytes(UBRIDGE_UART_PORT_NUM, buf, rx);
            uart_flush(UBRIDGE_UART_PORT_NUM);
        }

        // UART->USB
        rx = uart_read_bytes(UBRIDGE_UART_PORT_NUM, buf, sizeof(buf), 10 / portTICK_PERIOD_MS);
        //ESP_LOGI("UART->USB", "Read %d bytes", rx);
        usb_write(buf, rx);

        vTaskDelay(1); // minimal yield for watchdog
    }
}

