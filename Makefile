obj-m += pidriver.o 
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
 
PWD := $(CURDIR) 
 
all: client 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
client: client.c
	$(CC) -o $@ $^ -lm
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) client out