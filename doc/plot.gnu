set term pdfcairo font 'cmr10'
set lmargin at screen 0.09
set rmargin at screen 0.97
set bmargin at screen 0.12
set tmargin at screen 0.96

set yrange [2:18]
set xrange [0:20]

set key right bottom
set output 'plot.pdf'

set ylabel "Time (log(s))"
set xlabel "N"

#set title "Log of time (log(s))" font 'cmr10' 20
plot 'dump.dat' using 2:(log($4+1)) every :::0::0 title "g=0" with linespoint ls 1 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::1::1 title "g=1" with linespoint ls 2 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::2::2 title "g=2" with linespoint ls 3 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::3::3 title "g=3" with linespoint ls 4 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::4::4 title "g=4" with linespoint ls 5 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::5::5 title "g=5" with linespoint ls 8 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::6::6 title "g=6" with linespoint ls 7 pt 1,\
     'dump.dat' using 2:(log($4+1)) every :::7::7 title "g=7" with linespoint ls 9 pt 1
