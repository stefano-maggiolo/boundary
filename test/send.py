#!/usr/bin/env python

import os
import glob

os.system("gnuplot plot.gnu")
for x in glob.glob("*.png"):
    os.system("upload.sh %s" % x)

s = """
<html>
 <head>
  <title>Strata2 benchmark</title>
  <style type="text/css">
   h1 { text-align: center; }
   table { margin: 0px auto; border-spacing: 10px; }
   hr { margin: 20px auto; width: 900px; }
</style>
 </head>
 <body>
  <h1>Benchmark 0: (g, 2(5-g)).</h1>
  <table>
   <tr>
    <td><img src="./plot_0_time_log.png"</td>
    <td><img src="./plot_0_time_lin.png"</td>
   </tr>
   <tr>
    <td><img src="./plot_0_memory_log.png"</td>
    <td><img src="./plot_0_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 1: (g, 0).</h1>
  <table>
   <tr>
    <td><img src="./plot_1_time_log.png"</td>
    <td><img src="./plot_1_time_lin.png"</td>
   </tr>
   <tr>
    <td><img src="./plot_1_memory_log.png"</td>
    <td><img src="./plot_1_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 2: (0, n).</h1>
  <table>
   <tr>
    <td><img src="./plot_2_time_log.png"</td>
    <td><img src="./plot_2_time_lin.png"</td>
   </tr>
   <tr>
    <td><img src="./plot_2_memory_log.png"</td>
    <td><img src="./plot_2_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 3: (g, n).</h1>
  <table>
   <tr>
    <td><img src="./plot_3_time_log.png"</td>
    <td><img src="./plot_3_time_lin.png"</td>
   </tr>
   <tr>
    <td><img src="./plot_3_memory_log.png"</td>
    <td><img src="./plot_3_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 4: (g, n).</h1>
  <table>
   <tr>
    <td><img src="./plot_4_time_gn.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 5: Time &amp; memory.</h1>
  <table>
   <tr>
    <td><img src="./plot_5_time.png"</td>
    <td><img src="./plot_5_memory.png"</td>
   </tr>
   <tr>
    <td><img src="./plot_5_time_stable.png"</td>
    <td><img src="./plot_5_memory_stable.png"</td>
   </tr>
   <tr>
    <td><img src="./plot_5_time_total.png"</td>
    <td><img src="./plot_5_memory_total.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 6: Percentages.</h1>
  <table>
   <tr>
    <td><img src="./plot_6_unconnected.png"</td>
    <td><img src="./plot_6_duplicated.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 7: Totals.</h1>
  <table>
   <tr>
    <td><img src="./plot_7_stable.png"</td>
    <td><img src="./plot_7_total.png"</td>
   </tr>
  </table>
  <hr />
  
 </body>
</html>
"""

f = open("plot.html", "w")
f.write(s)
f.close()
os.system("upload.sh plot.html")
