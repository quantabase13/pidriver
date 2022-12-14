set xlabel 'N'
set ylabel 'Time(nanosec)'
set datafile separator ' '
set style fill solid
set title 'compute PI time'
set term png enhanced font 'Verdana,10'
set output 'time.png'
plot [0:1000][0:] \
'result_clock_gettime.csv' using 1:2 with linespoints title 'baseline',\
