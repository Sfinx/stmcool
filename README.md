
How to flash buttefly in DFU mode:
==================================

Press both buttons and release only RESET one. In dmesg/lsusb you will see:

```
usb 3-5: new full-speed USB device number 125 using xhci_hcd
usb 3-5: New USB device found, idVendor=0483, idProduct=df11
usb 3-5: New USB device strings: Mfr=1, Product=2, SerialNumber=3
usb 3-5: Product: STM32  BOOTLOADER
usb 3-5: Manufacturer: STMicroelectronics
usb 3-5: SerialNumber: 207F32B75841
```
```
Bus 003 Device 125: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
```

Then issue 'make dfu'
