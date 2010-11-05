#!/usr/bin/env python

from pygooglechart import *
from math import log

# series[i] = s
# s = [(x,y)...] withx values sorted
def plot(xAxis, series, name, logarithmic = True, scale = 1.0, title = "", legend = None):
    top = 0.0
    for s in series:
        for i in range(len(s)):
            if logarithmic:
                if s[i] > 0 and log(s[i]) > top: top = log(s[i])
            else:
                if s[i]/scale > top: top = s[i]/scale
    top = int(top + 1)

    chart = SimpleLineChart(600, 400, y_range=[0,top], title = title)

    def proc(x):
        if logarithmic:
            if x <= 0: return 0
            else: return log(x)
        else:
            return x/scale

    for s in series:
        chart.add_data([proc(x) for x in s])
    colours = ["0000FF", "00FF00", "FF0000", "FFFF00", "FF00FF", "00FFFF"]
    chart.set_colours((colours * (len(series) / len(colours) + 1))[:len(series)])


    if legend != None:
        chart.set_legend(legend)
        chart.set_legend_position("b")

    if logarithmic:
        left_axis = range(0, top)
    else:
        left_axis = range(0, top, int(top / 10.0 + 1))

    left_axis[0] = ''
    chart.set_axis_labels(Axis.LEFT, left_axis)

    if len(xAxis) > 16:
        xAxis = [xAxis[i] for i in range(0, len(xAxis), 4)]
    elif len(xAxis) > 8:
        xAxis = [xAxis[i] for i in range(0, len(xAxis), 2)]

    chart.set_axis_labels(Axis.BOTTOM, xAxis)

    chart.fill_linear_stripes(Chart.CHART, 0, 'CCCCCC', 1.0/(len(xAxis)-1), 'FFFFFF', 1.0/(len(xAxis)-1))
    chart.set_grid(0, 101.0/(len(left_axis)-1), 5, 5)

    chart.download(name)
