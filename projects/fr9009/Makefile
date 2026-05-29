IIOD ?= n
HARDWARE ?= system_top.xsa

ifneq ($(strip $(FR9009_DEVICE)),)
CFLAGS += -DFR9009_DEVICE=$(FR9009_DEVICE)
endif

.PHONY: xsa_build
xsa_build: all

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk