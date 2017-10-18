CMSIS       ?=../../CMSIS
FLASH       ?= st-flash
TOOLSET     ?= arm-none-eabi-
CC           = $(TOOLSET)gcc
LD           = $(TOOLSET)gcc
AR           = $(TOOLSET)gcc-ar
OBJCOPY      = $(TOOLSET)objcopy


STARTUP.stm32l052x8  = $(CMSIS)/Device/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l052xx.s
CFLAGS.stm32l052x8   = -mcpu=cortex-m0plus -mfloat-abi=soft
DEFINES.stm32l052x8  = STM32L0 STM32L052xx
LDSCRIPT.stm32l052x8 = demo/stm32l052x8.ld

STARTUP.stm32f072xb  = $(CMSIS)/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f072xb.s
CFLAGS.stm32f072xb   = -mcpu=cortex-m0 -mfloat-abi=soft
DEFINES.stm32f072xb  = STM32F0 STM32F072xB 
LDSCRIPT.stm32f072xb = demo/stm32f072xb.ld

STARTUP.stm32l100xc  = $(CMSIS)/Device/ST/STM32L1xx/Source/Templates/gcc/startup_stm32l100xc.s
CFLAGS.stm32l100xc   = -mcpu=cortex-m3 -mfloat-abi=soft
DEFINES.stm32l100xc  = STM32L1 STM32L100xC
LDSCRIPT.stm32l100xc = demo/stm32l100xc.ld

STARTUP.stm32l476rg  = $(CMSIS)/Device/ST/STM32L4xx/Source/Templates/gcc/startup_stm32l476xx.s
CFLAGS.stm32l476rg   = -mcpu=cortex-m4
DEFINES.stm32l476rg  = STM32L4 STM32L476xx
LDSCRIPT.stm32l476rg = demo/stm32l476xg.ld


MCU         ?= stm32l100xc
#MCU         ?= stm32f072xb
LDFLAGS     ?= --specs=nano.specs -nostartfiles -Wl,--gc-sections 
DSRC         = $(wildcard demo/*.c) $(wildcard demo/*.S) $(STARTUP.$(MCU))
DOBJ         = $(call fixpath, $(addsuffix .o, $(basename $(DSRC))))
DOUT         = cdc-loop
LDSCRIPT     = $(call fixpath, $(LDSCRIPT.$(MCU)))

OPTFLAG      = -Os  -flto
#OPTFLAG      = -O0  -g3
MODULE      ?= libusb_stm32.a
CFLAGS      ?= $(CFLAGS.$(MCU))
CFLAGS2      = -mthumb -std=gnu99 $(OPTFLAG)
DEFINES     ?= $(DEFINES.$(MCU)) FORCE_C_DRIVER
INCLUDES    ?= $(CMSIS)/Device/ST $(CMSIS)/Include .
SOURCES      = $(wildcard src/*.c) $(wildcard src/*.S)
OBJECTS      = $(addsuffix .o, $(basename $(SOURCES)))
ARFLAGS     ?= -cvq

ifeq ($(OS),Windows_NT)
	RM = del /Q
	fixpath = $(strip $(subst /,\, $1))
else
	RM = rm -f
	fixpath = $(strip $1)
endif



program: $(DOUT).hex
	$(FLASH) --reset --format ihex write $(DOUT).hex

demo: clean $(DOUT).hex

$(DOUT).hex : $(DOUT).elf
	@echo building $@
	@$(OBJCOPY) -O ihex $< $@

$(DOUT).elf : $(DOBJ) $(OBJECTS)
	@echo building $@ for $(MCU)
	@$(LD) $(CFLAGS) $(CFLAGS2) $(LDFLAGS) -Wl,--script='$(LDSCRIPT)' -Wl,-Map=$(DOUT).map $(DOBJ) -lc $(OBJECTS) -o $@

clean:
	$(RM) $(DOUT).* $(OBJECTS)

doc:
	doxygen

module: $(MODULE)


$(MODULE): $(OBJECTS)
	@$(AR) $(ARFLAGS) $(MODULE) $(OBJECTS)

%.o: %.c
	@echo compiling $@
	@$(CC) $(CFLAGS) $(CFLAGS2) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -c $< -o $@

%.o: %.S
	@echo assembling $@
	@$(CC) $(CFLAGS) $(CFLAGS2) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -c $< -o $@

%.o: %.s
	@echo assembling $@
	@$(CC) $(CFLAGS) $(CFLAGS2) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -c $< -o $@


.INTERMEDIATE: $(OBJECTS) $(DOBJ)

.PHONY: module doc demo clean program
