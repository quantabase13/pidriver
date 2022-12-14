obj-m += pidriver.o 
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
 
PWD := $(CURDIR) 
 
all: client 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
client: client.c
	$(CC) -o $@ $^ -lm
gencsv: client
		for i in `seq 1000 1000 100000`; do \
                printf "%d" $$i;\
			for j in `seq 0 1 49`; do\
				printf " ";\
                sudo ./client $$i;\
			done;\
		printf "\n";\
		done > result_clock_gettime.csv
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) client out
	$(RM) result_clock_gettime.csv