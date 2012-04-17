set xlabel "Number of Inserts (in multiples of 128)"
set ylabel "Time taken (in logscale)"
set terminal png
set output 'deamortised.png'
set logscale y
plot 'deamortized.dat' with lines
set terminal png
set output 'normal.png'
set logscale y
plot 'normal.dat' with lines
