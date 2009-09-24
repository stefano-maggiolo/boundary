#!/usr/bin/env python

import os

for i in xrange(4):
    for j in ["memory", "time"]:
        for k in ["lin", "log"]:
            os.system("upload.sh plot_%d_%s_%s.png" % (i, j, k))

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
<!--   <tr>
    <td></td>
    <td>Logarithmic</td>
    <td>Linear</td>
   </tr>-->
   <tr>
<!--    <td>Time</td>-->
    <td><img src="./plot_0_time_log.png"</td>
    <td><img src="./plot_0_time_lin.png"</td>
   </tr>
   <tr>
<!--    <td>Memory</td>-->
    <td><img src="./plot_0_memory_log.png"</td>
    <td><img src="./plot_0_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 1: (g, 0).</h1>
  <table>
<!--   <tr>
    <td></td>
    <td>Logarithmic</td>
    <td>Linear</td>
   </tr>-->
   <tr>
<!--    <td>Time</td>-->
    <td><img src="./plot_1_time_log.png"</td>
    <td><img src="./plot_1_time_lin.png"</td>
   </tr>
   <tr>
<!--    <td>Memory</td>-->
    <td><img src="./plot_1_memory_log.png"</td>
    <td><img src="./plot_1_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 2: (0, n).</h1>
  <table>
<!--   <tr>
    <td></td>
    <td>Logarithmic</td>
    <td>Linear</td>
   </tr>-->
   <tr>
<!--    <td>Time</td>-->
    <td><img src="./plot_2_time_log.png"</td>
    <td><img src="./plot_2_time_lin.png"</td>
   </tr>
   <tr>
<!--    <td>Memory</td>-->
    <td><img src="./plot_2_memory_log.png"</td>
    <td><img src="./plot_2_memory_lin.png"</td>
   </tr>
  </table>
  <hr />
  
  <h1>Benchmark 3: (g, n).</h1>
  <table>
<!--   <tr>
    <td></td>
    <td>Logarithmic</td>
    <td>Linear</td>
   </tr>-->
   <tr>
<!--    <td>Time</td>-->
    <td><img src="./plot_3_time_log.png"</td>
    <td><img src="./plot_3_time_lin.png"</td>
   </tr>
   <tr>
<!--    <td>Memory</td>-->
    <td><img src="./plot_3_memory_log.png"</td>
    <td><img src="./plot_3_memory_lin.png"</td>
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
