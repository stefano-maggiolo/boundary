set term png
set lmargin at screen 0.06
set rmargin at screen 0.88
set bmargin at screen 0.09
set tmargin at screen 0.9
unset key

set pm3d map
set pm3d interpolate 20,20
set pm3d flush begin
set pm3d ftriangle

set cbrange [0:100]

set output 'plot_6_duplicated.png'
set title "Percentage of duplicated graphs among the connected ones"
splot "sorted.dat" using 2:1:(100*$7/($5+$7+1))

set output 'plot_6_unconnected.png'
set title "Percentage of unconnected graphs"
splot "sorted.dat" using 2:1:(100*$6/($5+$6+$7+1))

set cbrange [0:18]

set output 'plot_7_stable.png'
set title "Log of number of stable graphs"
splot "sorted.dat" using 2:1:(log($5+1))

set output 'plot_7_total.png'
set title "Log of total number of graphs"
splot "sorted.dat" using 2:1:(log($5+$6+$7+1))

set autoscale

set output 'plot_5_time.png'
set title "Log of time"
splot "sorted.dat" using 2:1:(log($4+1))

set output 'plot_5_time_total.png'
set title "Log of time over total number of graphs"
splot "sorted.dat" using 2:1:(log(($4+1)/($5+$6+$7+1)))

set output 'plot_5_time_stable.png'
set title "Log of time over number of stable graphs"
splot "sorted.dat" using 2:1:(log(($4+1)/($5+1)))

set output 'plot_5_memory.png'
set title "Log of memory"
splot "sorted.dat" using 2:1:(log($3+1))

set output 'plot_5_memory_total.png'
set title "Log of memory over total number of graphs"
splot "sorted.dat" using 2:1:(log(($3+1)/($5+$6+$7+1)))

set output 'plot_5_memory_stable.png'
set title "Log of memory over number of stable graphs"
splot "sorted.dat" using 2:1:(log(($3+1)/($5+1)))

unset pm3d
set yrange [2:18]
set xrange [0:22]
lin(x) = a0*x + b0
a0 = 3
b0 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((21-$2)**3) every :::0::0 via a0, b0

lin(x) = a1*x + b1
a1 = 3
b1 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((15-$2)**3) every :::1::1 via a1, b1

lin(x) = a2*x + b2
a2 = 3
b2 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((12-$2)**3) every :::2::2 via a2, b2

lin(x) = a3*x + b3
a3 = 3
b3 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((8-$2)**3) every :::3::3 via a3, b3

lin(x) = a4*x + b4
a4 = 3
b4 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((6-$2)**3) every :::4::4 via a4, b4

lin(x) = a5*x + b5
a5 = 3
b5 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((4-$2)**3) every :::5::5 via a5, b5

lin(x) = a6*x + b6
a6 = 3
b6 = -30
fit lin(x) 'sorted.dat' using 2:(log($4+1)):((2-$2)**3) every :::6::6 via a6, b6

set key right bottom
set rmargin at screen 0.96
set output 'plot_4_time_gn.png'
set title "Log of time"
plot 'sorted.dat' using 2:(log($4+1)) every :::0::0 with linespoint title sprintf("g=0, a=%.2f", a0),\
     'sorted.dat' using 2:(log($4+1)) every :::1::1 with linespoint title sprintf("g=1, a=%.2f", a1),\
     'sorted.dat' using 2:(log($4+1)) every :::2::2 with linespoint title sprintf("g=2, a=%.2f", a2),\
     'sorted.dat' using 2:(log($4+1)) every :::3::3 with linespoint title sprintf("g=3, a=%.2f", a3),\
     'sorted.dat' using 2:(log($4+1)) every :::4::4 with linespoint title sprintf("g=4, a=%.2f", a4),\
     'sorted.dat' using 2:(log($4+1)) every :::5::5 with linespoint title sprintf("g=5, a=%.2f", a5),\
     'sorted.dat' using 2:(log($4+1)) every :::6::6 with linespoint title sprintf("g=6, a=%.2f", a6),\
     'sorted.dat' using 2:(log($4+1)) every :::7::7 with linespoint title "g=7",\
     a0*x+b0 notitle,\
     a1*x+b1 notitle,\
     a2*x+b2 notitle,\
     a3*x+b3 notitle,\
     a4*x+b4 notitle,\
     a5*x+b5 notitle 
