obj-m += pidriver.o 
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
 
PWD := $(CURDIR) 
 
all: client 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
client: client.c
	$(CC) -o $@ $^ -lm
gencsv: client
		for i in `seq 100000 5000 20000000`; do \
                printf "%d " $$i;\
                sudo ./client $$i; \
        done > result_clock_gettime.csv 
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) client out
	$(RM) result_clock_gettime.csv