# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
# variable takes the form <module executable>-objs
scull-objs := main.o

# obj-<m -> module, y-> builtin>
obj-m := scull.o

else
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
# /kernel/linux-headers... would be replaced with /lib/modules/$(shell uname -r)/build if not
# running in this container
KERNELDIR ?= /kernel/linux-headers-$(shell uname -r)
PWD       := $(shell pwd)

default:
# calls make again and now kernel build system will be invoked and KERNELRELEASE set
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
endif