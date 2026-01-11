// usb_direct.h
#ifndef USB_DIRECT_H
#define USB_DIRECT_H

// Direct USB JTAG write, no interrupts, no driver.
int usb_write(uint8_t *buf, uint32_t len);

// Direct USB JTAG read, no interrupts, no driver.
int usb_read(uint8_t *buf, uint32_t len);

void usb_clear_buffers(void);

#endif // USB_DIRECT_H