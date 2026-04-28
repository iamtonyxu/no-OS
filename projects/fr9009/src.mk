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
CFLAGS += -DFILE_SYSTEM_INTERFACE_SD \
        -DFILE_SYSTEM_USE_MKFS

SRCS += $(PROJECT)/src/app/ad9528_app.c \
	$(PROJECT)/src/driver/ad9528/ad9528.c \
	$(PROJECT)/src/driver/fr9009/app/src/FR_app.c \
	$(PROJECT)/src/driver/fr9009/app/src/FR_check.c \
	$(PROJECT)/src/driver/fr9009/fr_hal/src/no_os_hal.c \
	$(PROJECT)/src/driver/fr9009/init/src/fr9009_config.c \
	$(PROJECT)/src/driver/fr9009/init/src/headless.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_agc.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_arm_spi_cmd.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_arm.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_cals.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_error.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_gpio.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_hal.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_jesd204.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_radioctrl.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_rx.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_tx.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009_user.c \
	$(PROJECT)/src/driver/fr9009/talise/src/fr9009.c \
	$(PROJECT)/src/driver/xilinx_hal/platform_drivers.c \
	$(PROJECT)/src/main.c

INCS +=	$(PROJECT)/src/app/ad9528_app.h \
	$(PROJECT)/src/driver/ad9528/ad9528.h \
	$(PROJECT)/src/driver/ad9528/t_ad9528.h \
	$(PROJECT)/src/driver/fr9009/fr_hal/include/fr_hal.h \
	$(PROJECT)/src/driver/fr9009/init/include/fr9009_config.h \
	$(PROJECT)/src/driver/fr9009/init/include/fr9009_firmware.h \
	$(PROJECT)/src/driver/fr9009/init/include/headless.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_agc_types.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_agc.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_arm_spi_cmd.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_arm_macros.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_arm_types.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_arm.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_cals_types.h\
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_cals.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_error_types.h\
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_error.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_gpio_types.h\
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_gpio.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_hal.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_jesd204_types.h\
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_jesd204.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_radioctrl_types.h\
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_radioctrl.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_reg_addr_macros.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_rx_types.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_rx.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_tx_types.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_tx.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_types.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_user.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009_version.h \
	$(PROJECT)/src/driver/fr9009/talise/include/fr9009.h \
	$(PROJECT)/src/driver/xilinx_hal/common.h \
	$(PROJECT)/src/driver/xilinx_hal/parameters.h \
	$(PROJECT)/src/driver/xilinx_hal/platform_drivers.h
