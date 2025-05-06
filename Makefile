obj-m += ssd1306_driver_module.o

ssd1306_driver_module-objs := ssd1306_driver.o ssd1306.o

PWD := $(shell pwd)

CROSS=/home/tuoiio/work/beagle_bone_black/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

KER_DIR=/home/tuoiio/work/beagle_bone_black/bb-kernel/KERNEL

all: 
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) modules

clean:
	make -C $(KER_DIR) M=$(PWD) clean