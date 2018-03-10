
JLINKDIR    = ../j-link/jlink/
# 100000
JLINKSPEED  = 800
JLINKDEVICE = STM32L433CC

TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex 
BIN  = $(CP) -O binary

MCU  = cortex-m3
RUN_FROM_FLASH = 1
USE_RTT = 1

# List all default C defines here, like -D_DEBUG=1
# set the DDEFS CPU & HAL usage
DDEFS = -DSTM32L433xx -DUSE_HAL_DRIVER

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = 

# List all default directories to look for include files here
DINCDIR = usb-device-lib/Core/Inc usb-device-lib/Class/CDC/Inc

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS = 

# Define project name and Ram/Flash mode here
PROJECT        = stmcool

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS = 

LIBSDIR      = ./lib
DRIVERS      = $(LIBSDIR)/drivers
DRIVERSINC   = $(LIBSDIR)/drivers/Inc
DRIVERSRC    = $(LIBSDIR)/drivers/Src
USBSRC       = ./usb-device-lib
CMSISDIR     = $(LIBSDIR)/CMSIS
CMSISDIRINC  = $(CMSISDIR)/Include
DEVDIR       = $(CMSISDIR)/Device/ST/STM32L4xx
DEVDIRINC    = $(DEVDIR)/Include
BOARD        = $(LIBSDIR)/board
BOARDINC     = $(BOARD)/inc
STARTUP      = $(BOARD)/startup
LINKER       = $(BOARD)

# source list

SRC  = app.c
SRC += lib.c
SRC += usbd_cdc_interface.c
SRC += usbd_conf.c
SRC += usbd_desc.c
SRC += handlers.c
SRC += syscalls.c

SRC += $(STARTUP)/system_stm32l4xx.c

SRC += $(DRIVERSRC)/stm32l4xx_hal.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_cortex.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_rcc.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_rcc_ex.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_pwr.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_pwr_ex.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_gpio.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_tim.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_adc.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_adc_ex.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_tim_ex.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_pcd.c
SRC += $(DRIVERSRC)/stm32l4xx_hal_pcd_ex.c
SRC += $(DRIVERSRC)/stm32l4xx_ll_usb.c
SRC += $(USBSRC)/Core/Src/usbd_core.c
SRC += $(USBSRC)/Core/Src/usbd_ctlreq.c
SRC += $(USBSRC)/Core/Src/usbd_ioreq.c
SRC += $(USBSRC)/Class/CDC/Src/usbd_cdc.c

#SRC += lib.c
#SRC += handlers.c
#SRC += $(CORELIBDIR)/core_cm3.c
#SRC += $(DEVDIR)/system_stm32f10x.c
#SRC += $(STARTUP)/crt.c 
#SRC += $(STARTUP)/newlib_stubs.c

#SRC += $(STMSPSRCDDIR)/stm32f10x_gpio.c
#SRC += $(STMSPSRCDDIR)/stm32f10x_rcc.c
#SRC += $(STMSPSRCDDIR)/stm32f10x_tim.c
#SRC += $(STMSPSRCDDIR)/stm32f10x_exti.c
#SRC += $(STMSPSRCDDIR)/misc.c

ifeq ($(USE_RTT), 1)
DDEFS   += -DUSE_RTT
DINCDIR += rtt/RTT
SRC     += rtt/Syscalls/SEGGER_RTT_Syscalls_GCC.c
SRC     += rtt/RTT/SEGGER_RTT.c
SRC     += rtt/RTT/SEGGER_RTT_printf.c
endif

# List ASM source files here
ASRC = $(DEVDIR)/Source/Templates/gcc/startup_stm32l433xx.s

# List all user directories here
UINCDIR  = $(DEVDIRINC)
UINCDIR += $(CMSISDIRINC)
UINCDIR += $(DRIVERSINC)
UINCDIR += $(BOARDINC)
# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

# Define optimisation level here
# -mfpu=vfp -msoft-float -fno-builtin
OPT = -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -D__thumb2__ -D__BUFSIZ__=256 -Os

ifeq ($(RUN_FROM_FLASH), 0)
LDSCRIPT = $(LINKER)/ram.ld
else
LDSCRIPT = $(LINKER)/flash.ld
endif

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))

ifeq ($(RUN_FROM_FLASH), 0)
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=0 -DVECT_TAB_SRAM
else
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=1
endif

ADEFS   = $(DADEFS) $(UADEFS)
OBJS    = $(ASRC:.s=.o) $(SRC:.c=.o)
LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU) -mthumb -flto -ffreestanding
#MCFLAGS += -msoft-float

ASFLAGS = $(MCFLAGS) -D__thumb2__ -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
#-ffreestanding
CPFLAGS = $(MCFLAGS) $(OPT) -Werror -fomit-frame-pointer -ffunction-sections -fdata-sections -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -fwhole-program -T$(LDSCRIPT) -Wl,--gc-sections,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR)

# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d

#
# makefile rules
#

all:	tty stmcool

stmcool: $(OBJS) $(PROJECT).elf $(PROJECT).bin
	@$(TRGT)size $(PROJECT).elf

tty: tty.cpp
	$(CXX) -O2 -Wall tty.cpp -o tty -lcommoncpp -lucommon

%.o : %.c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

clean:
	@rm -f $(OBJS)
	@rm -f *.elf *.map *.hex JLink.log tty
	@rm -f $(SRC:.c=.c.bak)
	@rm -f $(SRC:.c=.lst)
	@rm -f $(ASRC:.s=.s.bak)
	@rm -f $(ASRC:.s=.lst)
	@rm -fR .dep
	@echo Clean done

todo:
	@cat TODO
	@echo

dfu:
	@dfu-util --alt 0 -s 0x08000000:force:mass-erase:leave -D $(PROJECT).bin

stlink:	all
	@st-flash write $(PROJECT).bin 0x08000000

prg:	all
	@$(JLINKDIR)/JLinkExe -if SWD -speed $(JLINKSPEED) -device $(JLINKDEVICE) -AutoConnect 1 -CommandFile prg.cmd

dbg:	all
	@$(JLINKDIR)/JLinkExe -if SWD -speed $(JLINKSPEED) -device $(JLINKDEVICE) -AutoConnect 1 -CommandFile dbg.cmd

view:
	@$(JLINKDIR)/JLinkRTTClient

push:
	@git push origin

revback:
	@git reset --soft HEAD^

-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
