obj-m = dot_matrix_driver.o push_switch_driver.o fpga_fnd_driver.o

PWD = $(shell pwd)
all: driver app

driver :
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app :
	gcc -W -fstack-protector -static -o test_dot_tetris test_dot_tetris.c

clean :
	rm -rf *.ko *.mod.* *.o test_dot_tetris Module.symvers modules.order.tetris*.tmp*


