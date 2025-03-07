ARCH_TYPE ?=
CROSS_COMPILE ?=
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Load GUI menu to select architecture
config:
	@echo "Launching GUI Config..."
	kconfig-mconf Kconfig

# Set architecture based on the choice
include .config

ifeq ($(CONFIG_ARCH_ARM), y)
    ARCH_TYPE := arm
    CROSS_COMPILE := arm-linux-androideabi-
endif
ifeq ($(CONFIG_ARCH_ARM64), y)
    ARCH_TYPE := arm64
    CROSS_COMPILE := aarch64-linux-android-
endif
ifeq ($(CONFIG_ARCH_X86_64), y)
    ARCH_TYPE := x86_64
    CROSS_COMPILE := x86_64-linux-android-
endif

all: config
	@echo "Building for $(ARCH_TYPE) using $(CROSS_COMPILE)"
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH_TYPE) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
