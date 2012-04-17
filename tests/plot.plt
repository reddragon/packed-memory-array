set xlabel "Insert number"
set ylabel "Time taken (in logscale)"
set terminal png
set output 'deamortised.png'
set logscale y
set logscale x 2
plot 'deamortized.dat' with lines
set terminal png
set output 'normal.png'
set logscale y
plot 'normal.dat' with lines
