set xlabel 'N'
set ylabel 'Time(nanosec)'
set datafile separator ' '
set style fill solid
set title 'compute PI time'
set term png enhanced font 'Verdana,10'
set output 'time2.png'
plot [:][:] \
'result_clock_gettime.csv' using 1:2 with linespoints title 'baseline',\
'result_clock_gettime.csv' using 1:3 with linespoints title '4 thread',\