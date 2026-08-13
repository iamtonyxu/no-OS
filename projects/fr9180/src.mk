################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

SRCS += $(PROJECT)/src/main.c \
	$(PROJECT)/src/platform.c

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_lf256fifo.c

SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_axi_io.c
SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c

ifeq (linux,$(strip $(PLATFORM)))
SRCS +=	$(PLATFORM_DRIVERS)/linux_delay.c
else
SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c
endif

INCS += $(PROJECT)/src/platform_config.h \
	$(PROJECT)/src/platform.h

ifeq (linux,$(strip $(PLATFORM)))
INCS +=	$(PLATFORM_DRIVERS)/linux_spi.h \
	$(PLATFORM_DRIVERS)/linux_gpio.h \
	$(PLATFORM_DRIVERS)/linux_uart.h
endif

INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h

ifeq (xilinx,$(strip $(PLATFORM)))
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c

INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
