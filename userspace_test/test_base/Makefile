client: pi_base.c
	$(CC) -o $@ $^ -lpthread
gencsv: client
		for i in `seq 10000 10000 1000000`; do \
                printf "%d" $$i;\
			# for j in `seq 0 1 49`; do\
				printf " ";\
                ./client $$i 1;\
				printf " ";\
                ./client $$i 4;\
			# done;\
		printf "\n";\
		done > result_clock_gettime.csv

clean:
	rm client