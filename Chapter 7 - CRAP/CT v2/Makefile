mod-name := kmod_v2
obj-m += $(mod-name).o
PWD=$(shell pwd)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

.PHONY:
install: all
	sudo insmod $(mod-name).ko

.PHONY:
remove:
	sudo rmmod $(mod-name).ko