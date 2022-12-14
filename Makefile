obj-m += pidriver.o 
ccflags-y := -std=gnu99 -Wno-declaration-after-statement -g
 
PWD := $(CURDIR) 
 
all: client 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
client: client.c
	$(CC) -o $@ $^ -lm
gencsv: client
		for i in `seq 1 1 1000`; do \
                printf "%d" $$i;\
			for j in `seq 0 1 49`; do\
				printf " ";\
                sudo taskset 0x8 ./client $$i;\
			done;\
		printf "\n";\
		done > result_clock_gettime.csv
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) client out
	$(RM) result_clock_gettime.csv