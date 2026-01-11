// main.c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hal/usb_serial_jtag_ll.h"

#include "usb_direct.h"

//#define TRACE_IO

static const char* TAG = "usb_direct";


void usb_clear_buffers(void) {
    // read all pending to read bytes
    uint8_t buf[1];
    while (usb_read(buf, 1));

    // flush all pending to write bytes
    usb_serial_jtag_ll_txfifo_flush();
}


// Direct USB JTAG write, no interrupts, no driver.
// It is blocking write. So it must return exacly the len asked.
// If it returns less, it was due to timeout.
int usb_write(uint8_t *buf, uint32_t len) {
    if (!buf || len == 0) return 0;

    uint32_t total_written = 0;
    const uint8_t *pos = buf;

    uint32_t timeouts = 0;
    const uint32_t TIMEOUT_MAX = 100; // avoid infinite loop (e.g., if HW stuck)
    const uint32_t TIMEOUT_MS  =  20; // avoid infinite loop (e.g., if HW stuck)

#ifdef TRACE_IO
    ESP_LOGI(TAG, "USB->Host %d bytes", len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buf, len, ESP_LOG_INFO);
#endif

    while (total_written < len) {
        int written_now = usb_serial_jtag_ll_write_txfifo(pos, len - total_written);
        usb_serial_jtag_ll_txfifo_flush();
        if (written_now > 0) {
            total_written += written_now;
            pos += written_now;
            timeouts = 0;
        } else {
            vTaskDelay(TIMEOUT_MS / portTICK_PERIOD_MS);
            if (++timeouts > TIMEOUT_MAX) {
                ESP_LOGW(TAG, "TX timeout after %u ms", TIMEOUT_MAX*TIMEOUT_MS);
                break;
            }
        }
    }
    return (int)total_written;
}

// Direct USB JTAG read, no interrupts, no driver.
int usb_read(uint8_t *buf, uint32_t len) {
    int rx = usb_serial_jtag_ll_read_rxfifo(buf, len);
#ifdef TRACE_IO
    if (rx > 0) {
        ESP_LOGI(TAG, "USB<-Host %d bytes", rx);
        ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx, ESP_LOG_INFO);
    }
#endif
    return rx;
}


















