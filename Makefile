
JLINKDIR = ../j-link/jlink/
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex 
BIN  = $(CP) -O binary

MCU  = cortex-m3
RUN_FROM_FLASH = 1
USE_RTT = 1

#     STM32F10X      FLASH Size  SRAM Size  Device Type
#   -------------------------------------------------------
#     STM32F100x4    16 KB       4 KB       STM32F10X_LD_VL
#     STM32F100x6    32 KB       4 KB       STM32F10X_LD_VL
#     STM32F100x8    64 KB       8 KB       STM32F10X_MD_VL
#     STM32F100xB    128 KB      8 KB       STM32F10X_MD_VL
#     STM32F100xC    256 KB      24 KB      STM32F10X_HD_VL
#     STM32F100xD    384 KB      32 KB      STM32F10X_HD_VL
#     STM32F100xE    512 KB      32 KB      STM32F10X_HD_VL
#   ----------------------------------------------------
#     STM32F101x4    16 KB       4 KB       STM32F10X_LD
#     STM32F101x6    32 KB       6 KB       STM32F10X_LD
#     STM32F101x8    64 KB       10 KB      STM32F10X_MD
#     STM32F101xB    128 KB      16 KB      STM32F10X_MD
#     STM32F101xC    256 KB      32 KB      STM32F10X_HD
#     STM32F101xD    384 KB      48 KB      STM32F10X_HD
#     STM32F101xE    512 KB      48 KB      STM32F10X_HD
#     STM32F101xF    768 KB      80 KB      STM32F10X_XL
#     STM32F101xG    1024 KB     80 KB      STM32F10X_XL
#   ----------------------------------------------------
#     STM32F102x4    16 KB       4 KB       STM32F10X_LD
#     STM32F102x6    32 KB       6 KB       STM32F10X_LD
#     STM32F102x8    64 KB       10 KB      STM32F10X_MD
#     STM32F102xB    128 KB      16 KB      STM32F10X_MD
#   ----------------------------------------------------
#     STM32F103x4    16 KB       6 KB       STM32F10X_LD
#     STM32F103x6    32 KB       10 KB      STM32F10X_LD
#     STM32F103x8    64 KB       20 KB      STM32F10X_MD
#     STM32F103xB    128 KB      20 KB      STM32F10X_MD
#     STM32F103xC    256 KB      48 KB      STM32F10X_HD
#     STM32F103xD    384 KB      64 KB      STM32F10X_HD
#     STM32F103xE    512 KB      64 KB      STM32F10X_HD
#     STM32F103xF    768 KB      96 KB      STM32F10X_XL
#     STM32F103xG    1024 KB     96 KB      STM32F10X_XL
#   ----------------------------------------------------
#     STM32F105x8    64 KB       64 KB      STM32F10X_CL
#     STM32F105xB    128 KB      64 KB      STM32F10X_CL
#     STM32F105xC    256 KB      64 KB      STM32F10X_CL
#   ----------------------------------------------------
#     STM32F107xB    128 KB      64 KB      STM32F10X_CL
#     STM32F107xC    256 KB      64 KB      STM32F10X_CL

# set the DDEFS CPU define !!!!!
# List all default C defines here, like -D_DEBUG=1
DDEFS = -DSTM32F10X_MD_VL -DUSE_STDPERIPH_DRIVER

ifeq ($(USE_RTT), 1)
DDEFS += -DUSE_RTT
endif

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = 

# List all default directories to look for include files here
DINCDIR = rtt/RTT

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

LIBSDIR    = ./lib
CORELIBDIR = $(LIBSDIR)/CMSIS/CM3/CoreSupport
DEVDIR  = $(LIBSDIR)/CMSIS/CM3/DeviceSupport/ST/STM32F10x
STMSPDDIR    = $(LIBSDIR)/STM32F10x_StdPeriph_Driver
STMSPSRCDDIR = $(STMSPDDIR)/src
STMSPINCDDIR = $(STMSPDDIR)/inc
DRIVERS    = $(LIBSDIR)/Drivers
BOARD       = ./lib/board
BOARDINC    = $(BOARD)/inc
STARTUP      = $(BOARD)/startup
LINKER       = $(BOARD)

# source list

SRC  = app.c
SRC += lib.c
SRC += handlers.c
SRC += $(CORELIBDIR)/core_cm3.c
SRC += $(DEVDIR)/system_stm32f10x.c
SRC += $(STARTUP)/crt.c 
SRC += $(STARTUP)/vectors_stm32f10x.c
SRC += $(STARTUP)/newlib_stubs.c

SRC += $(STMSPSRCDDIR)/stm32f10x_gpio.c
SRC += $(STMSPSRCDDIR)/stm32f10x_rcc.c
SRC += $(STMSPSRCDDIR)/stm32f10x_tim.c
SRC += $(STMSPSRCDDIR)/stm32f10x_exti.c
SRC += $(STMSPSRCDDIR)/misc.c

ifeq ($(USE_RTT), 1)
SRC += rtt/Syscalls/SEGGER_RTT_Syscalls_GCC.c
SRC += rtt/RTT/SEGGER_RTT.c
SRC += rtt/RTT/SEGGER_RTT_printf.c
endif

# List ASM source files here
ASRC =

# List all user directories here
UINCDIR  = $(CORELIBDIR)
UINCDIR += $(DEVDIR)
UINCDIR += $(STMSPINCDDIR)
UINCDIR += $(LEDS)
UINCDIR += $(BUTTONS)   
UINCDIR += $(USART)       
UINCDIR += $(BOARDINC)
# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS = -lm

# Define optimisation level here
# -mfpu=vfp -msoft-float -fno-builtin
OPT = -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -D__thumb2__ -D__BUFSIZ__=256 -Os

ifeq ($(RUN_FROM_FLASH), 0)
LDSCRIPT = $(LINKER)/stm32-ram.ld
else
LDSCRIPT = $(LINKER)/stm32-flash.ld
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
#MCFLAGS = -mcpu=$(MCU) -mthumb -msoft-float -flto
MCFLAGS = -mcpu=$(MCU) -mthumb

ASFLAGS = $(MCFLAGS) -D__thumb2__ -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
#-ffreestanding
CPFLAGS = $(MCFLAGS) $(OPT) -Werror -fomit-frame-pointer -ffunction-sections -fdata-sections -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -fwhole-program -nostartfiles -T$(LDSCRIPT) -Wl,--gc-sections,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR)

# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d

#
# makefile rules
#

all: $(OBJS) $(PROJECT).elf $(PROJECT).bin
	$(TRGT)size $(PROJECT).elf

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
	-rm -f $(OBJS)
	-rm -f *.elf *.bin *.map *.hex JLink.log
	-rm -f $(SRC:.c=.c.bak)
	-rm -f $(SRC:.c=.lst)
	-rm -f $(ASRC:.s=.s.bak)
	-rm -f $(ASRC:.s=.lst)
	-rm -fR .dep

todo:
	@cat TODO
	@echo

prg:	all
	-$(JLINKDIR)/JLinkExe -if SWD -speed 4000 -device stm32f100rb -AutoConnect 1 -CommandFile prg.cmd

dbg:	all
	-$(JLINKDIR)/JLinkExe -if SWD -speed 4000 -device stm32f100rb -AutoConnect 1 -CommandFile dbg.cmd

view:
	-$(JLINK)/JLinkRTTClient

-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
