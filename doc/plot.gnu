set term pdfcairo font 'cmr10'

set log y

set xrange [-1:22]
set key right bottom
set output 'plot.pdf'

set ylabel "Time (s)"
set xlabel "N"

plot 'dump.dat' using 2:($4/1000.0) every :::0::0 title "g=0" with linespoint ls 1 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::1::1 title "g=1" with linespoint ls 2 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::2::2 title "g=2" with linespoint ls 3 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::3::3 title "g=3" with linespoint ls 4 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::4::4 title "g=4" with linespoint ls 5 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::5::5 title "g=5" with linespoint ls 8 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::6::6 title "g=6" with linespoint ls 7 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::7::7 title "g=7" with linespoint ls 9 pt 1,\
     'dump.dat' using 2:($4/1000.0) every :::8::8 title "g=8" with linespoint ls 10 pt 1
