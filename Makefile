obj-m += pidriver.o 
ccflags-y := -std=gnu99 -Wno-declaration-after-statement -g
 
PWD := $(CURDIR) 
 
all: client 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
client: client.c
	$(CC) -o $@ $^ -lm
gencsv: client
		for i in `seq 100000 100000 100000000`; do \
                printf "%d" $$i;\
			# for j in `seq 0 1 49`; do\
				printf " ";\
                sudo taskset 0x1 ./client $$i 1;\
				printf " ";\
                sudo taskset 0xf ./client $$i 4;\
			# done;\
		printf "\n";\
		done > result_clock_gettime.csv

gencsvpi: client
		for i in `seq 10000 10000 10000000`; do \
                printf "%d" $$i;\
			# for j in `seq 0 1 49`; do\
				printf " ";\
                sudo taskset 0x8 ./client $$i 0;\
			# done;\
		printf "\n";\
		done > pi.csv
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) client out
	$(RM) result_clock_gettime.csv