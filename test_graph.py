#!/usr/bin/env python

from pygooglechart import *
from math import log

# series[i] = s
# s = [(x,y)...] withx values sorted
def plot(xAxis, series, name, logarithmic = True, title = "", legend = None):
    top = 0
    for s in series:
        for i in range(len(s)):
            if logarithmic:
                if s[i] == 0: s[i] = 0
                else: s[i] = log(s[i])
            if s[i] > top: top = s[i]
    top = int(top + 1)

    chart = SimpleLineChart(600, 400, y_range=[0,top], title = title)

    colours = ["0000FF", "00FF00", "FF0000", "FFFF00", "FF00FF", "00FFFF"]
    for s in series:
        chart.add_data(s)
    chart.set_colours((colours * (len(series) / len(colours) + 1))[:len(series)])

    chart.fill_linear_stripes(Chart.CHART, 0, 'CCCCCC', 0.2, 'FFFFFF', 0.2)

    chart.set_grid(0, 20, 5, 5)

    if legend != None:
        chart.set_legend(legend)

    if logarithmic:
        left_axis = range(0, top)
    else:
        left_axis = range(0, top, int(top / 1000.0) * 100)
        
    left_axis[0] = ''
    chart.set_axis_labels(Axis.LEFT, left_axis)
    chart.set_axis_labels(Axis.BOTTOM, xAxis)

    chart.download(name)
