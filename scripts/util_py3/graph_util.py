#! /usr/bin/python

# Natacha Crooks - UC Berkeley - 2020
# Contains plotting functions


####### Important Functions include:
# plotLine()
# plotCDF()
# plotStackedBars()
# plotBars()
# plotBarsDouble()
# Documentation for each function is included in the function definition


import matplotlib
matplotlib.use('Agg')
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import sys
import os
import subprocess
import _thread
import time
import datetime
from matplotlib import font_manager
from matplotlib import mlab

# Patterns for Black/White visibility
patterns = ['//','\\','/', '//', '-', '+', 'x', '-','\\', '*', 'o', 'O', '.']
# Colour of bars
colours = [
'#001429',
'#002952',
'#003D7A',
'#0052A3',
'#0066CC',
'#3385D6',
'#99C2EB',
'#66A3E0',
'#CCE0F5',
] * 5
colours.reverse()

## Pass in the title of the graph,
## the label of the x axis
## the label of the y axis
## where the graph should be saved
## a list of tuples of the form (data.txt,label,x_index,y_index)
## whether it should be a line or points
def plotLine(title, x_axis, y_axis, save, data, points, legend='upper left', xleftlim = 0, xrightlim = 0, yleftlim = 0, yrightlim = 0,
            sizeFig=(4,4), paper=True, logX=False, logY=False,
            log2X=False, log2Y=False):
 try:
  if (paper):
      setPaperRC(sizeFig)
  else:
      setRC()

  fig,ax = plt.subplots()

  mark = ['-o','-v','-^','-D','-x','-D']

  i = 0
  for tup in data:
   data_path = tup[0]
   data =np.atleast_2d(np.loadtxt(data_path))
   lab = tup[1]
   x = tup[2]
   y = tup[3]
   data = data[data[:,0].argsort()]
   rows = data.shape[0]
   if points:
    plt.plot(data[:,x],data[:,y], mark[i],linewidth=2,label= lab,markersize=4)
   else:
    plt.plot(data[:,x],data[:,y], '-',linewidth=2,label=lab)
   i = i + 1

 # Output graphs

  print("Outputting graphs")
  plt.setp(ax.get_xticklabels(), rotation='vertical')

  if (logY):  ax.set_yscale('log')
  if (logX): ax.set_xscale('log')
  if (log2X):  ax.set_xscale('log', basex=2)
  if (log2Y): ax.set_yscale('log', basey=2)


  if(xleftlim):
      ax.set_xlim(left=xleftlim)
  if(xrightlim):
      ax.set_xlim(right=xrightlim)
  if(yleftlim):
      ax.set_ylim(bottom=yleftlim)
  if(yrightlim):
      ax.set_ylim(top=yrightlim)

  plt.xlabel(x_axis)
  plt.ylabel(y_axis)

  leg = plt.legend(loc=legend)
  frame  = leg.get_frame()
  frame.set_linewidth(0)
  frame.set_fill(None)
  plt.title(title)
  saveGraph(save)
  plt.close()
 except Exception as e:
     print(e)
     print("Could not generate graph")

## Pass in the title of the graph,
## the label of the x axis
## the label of the y axis
## where the graph should be saved
## a list of tuples of the form (data.txt,index)
def plotCDF(title, x_axis, y_axis, save, data, points, legend='upper left', sizeFig=(4,4), paper=True):

 try:
  if (paper):
      setPaperRC(sizeFig)
  else:
      setRC()

  fig,ax = plt.subplots()
  dat =np.atleast_2d(np.loadtxt(data[0]))

  # Output graphs
  n, bins, patches = ax.hist(dat[:,data[1]], n_bins, normed=1, histtype='step',
                                   cumulative=True, label='Empirical')
  plt.xlabel(x_axis)
  plt.ylabel(y_axis)

  leg = plt.legend()
  frame  = leg.get_frame()
  frame.set_linewidth(0)
  frame.set_fill(None)
  plt.title(title)
  saveGraph(save)
  plt.close()
 except Exception as e:
     print(e)
     print("Could not generate graph")



# Bar plot, where the input (data) is an array
# of array, represent the different values for
# different datasets. ex [[1a-1b-1c),2,3,4],
#                         [3,4,5,6]],
# will plot 4 clustered bars (1,3), (2,3),(3,5), where a,b,c will be stacked etc
# title is the tile of the graph
# barNames Bar cluster name ex: Read-Heavy, Write-Heavy
# datasetName Dataset Name ex: TARdiS, BerkeleyDB
## data: a list of tuples of the form (data.txt,index)
# dataStd:  a list of tuples of the form (data.txt,index)
# corresponds to a dataset errobar
# yAxis is the title of the y axis
# showvalues: if true, show value on top of bar
# save name of outputfile
##
def plotStackedBars(title, barNames, datasetName, yAxis, data_,showvalues,save,dataStd=[],legend='upper left',ylim=0, black=True,
             sizeFig=(4,4), paper=True, log=False):
 try:
  if (paper):
      setPaperRC(sizeFig)
  else:
      setRC(sizeFig)

  # Width of bars
  width = 0.10

  colours = [
'#FFFFFF',
'#003D7A',
'#66A3E0',
'#CCE0F5'
  ]


  data=[]
  plt.figure(figsize=(1,1))

  print(data_)
  for tup in data_:
   data_path = tup[0]
   values =np.atleast_2d(np.loadtxt(data_path, dtype="string", delimiter=" "))
   index = tup[1]
   print(index)
   vals = list(values[:,index])
   print(vals)
   ## Now we have ((1a,1b,1c), (3a,3b,3c))
   vals_ = list()
   for v in vals:
     vals_.append(v.split("-"))
   # Data is a list of a list
   data.append(vals_)


  N = len(barNames)
  ind = np.arange(N)
  fig,ax = plt.subplots()

  if (log):  ax.set_yscale('log')
#  plt.setp(ax.get_yticklabels(), rotation='horizontal', fontsize=30)
  if (ylim!=0):
      ax.set_ylim(top=ylim)
  i = 0
  j = 0 # Change colours
  print(datasetName)
  k = 0
  for d in range(0,len(data[0])):
     if (len(dataStd)==0):
        offset = i*width
        stacked = data[i]
        # Compute the n "subdataset"
        prev_spl = [0] * len(stacked)
        for j in range(0,len(stacked[0])):
            spl = list()
            for t in range(0,len(stacked)):
                spl.append(float(stacked[t][j]))
            bot = prev_spl
            print(i+j)
            if not black:
                rects = ax.bar(ind + offset, spl, width, bottom=bot,color=colours[k], label=datasetName[k])
            else:
                rects = ax.bar(ind + offset, spl, width, bottom=bot,color=colours[k], label=datasetName[k])
            prev_spl = spl
            if k == 3:
             k = 0
            else:
             k=k+1
     else:
         print("Not Yet Implemented")
     i = i + 1

  ax.set_ylabel(yAxis)
  ax.set_xticks(ind + width)
  ax.set_xticklabels(tuple(barNames))
  #legend = plt.legend(loc=legend)
  legend = plt.legend()
  frame  = legend.get_frame()
  frame.set_linewidth(0)
  frame.set_fill(None)
  #plt.setp(ax.get_xticklabels(), rotation='vertical')
  #plt.title(title)
  saveGraph(save)
  plt.close()
 except Exception as e:
     print(e)
     print("Could not generate graphs")


## Bar plot, where the input (data) is an array
# of array, represent the different values for
# different datasets. ex [[1,2,3,4],[3,4,5,6]],
# will plot 4 clustered bars (1,3), (2,3),(3,5), etc
# title is the tile of the graph
# barNames Bar cluster name ex: Read-Heavy, Write-Heavy
# datasetName Dataset Name ex: TARdiS, BerkeleyDB
# data: a list of tuples of the form (data.txt,index)
# dataStd:  a list of tuples of the form (data.txt,index)
# corresponds to a dataset errobar
# yAxis is the title of the y axis
# showvalues: if true, show value on top of bar
# save name of outputfile
##
def plotBars(title, barNames, datasetName, yAxis, data_,showvalues,save,dataStd_=[],legend='upper left',ylim=0, black=True,
             sizeFig=(4,4), paper=True, logX=False, logY=False, xAxis=None, xHor=False):
 try:
  if (paper):
      setPaperRC(sizeFig)
  else:
      setRC(sizeFig)

  # Width of bars
  if (len(datasetName) >=5):
    width = 0.15
  else:
    width = 0.20

  data=[];
  dataStd=[];

  for tup in data_:
   data_path = tup[0]
   values =np.atleast_2d(np.loadtxt(data_path))
   index = tup[1]
   data.append(np.array(values[:,index]));

  for tup in dataStd_:
   data_path = tup[0]
   values =np.atleast_2d(np.loadtxt(data_path))
   index = tup[1]
   dataStd.append(np.array(values[:,index]));


  N = len(barNames)
  ind = np.arange(N)
  fig,ax = plt.subplots()
  if (ylim!=0):
      ax.set_ylim(top=ylim)
  i = 0
  for d in datasetName:
     offset = i*width + 0.10
     if (len(dataStd)==0):
         if not black:
          rects = ax.bar(ind + offset, data[i] , width, bottom=0, color=colours[i],label=datasetName[i])
         else:
          rects = ax.bar(ind + offset, data[i] , width, bottom=0,color=colours[i], hatch=patterns[i], label=datasetName[i])

     else:
         if not black:
          print("Not balck")
          rects = ax.bar(ind + offset, data[i], width, bottom=0, yerr=(dataStd[i]), color=colours[i], label=datasetName[i])
         else:
          rects = ax.bar(ind + offset, data[i], width, bottom=0, yerr=(dataStd[i]), color=colours[i], hatch=patterns[i],label=datasetName[i])
     i = i + 1

  if (logY): ax.set_yscale('log')
  if (logX): ax.set_yscale('log')
  if (xHor): plt.setp(ax.get_xticklabels(), rotation='vertical')

  if xAxis != None:
    ax.set_xlabel(xAxis)
  ax.set_ylabel(yAxis)
  ax.set_xticks(0.20+ ind + width)
  ax.set_xticklabels(tuple(barNames))
  print(barNames)

  legend = plt.legend(
          bbox_transform=plt.gcf().transFigure)
  frame  = legend.get_frame()
  frame.set_linewidth(0)
  frame.set_fill(None)
  plt.title(title)
  saveGraph(save)
  plt.close()
 except Exception as e:
     print(e)
     print("Could not generate graphs")

## Bar plot, where the input (data) is an array
# of array, represent the different values for
# different datasets. ex [[1,2,3,4],[3,4,5,6]],
# will plot 4 clustered bars (1,3), (2,3),(3,5), etc
# title is the tile of the graph
# barNames Bar cluster name ex: Read-Heavy, Write-Heavy
# datasetName Dataset Name ex: TARdiS, BerkeleyDB
## data: a list of tuples of the form (data.txt,index)
# dataStd:  a list of tuples of the form (data.txt,index)
# corresponds to a dataset errobar
# yAxis is the title of the y axis
# showvalues: if true, show value on top of bar
# save name of outputfile
##
def plotBarsDouble(title, barNames, datasetName, yAxis1, yAxis2,  data_,showvalues,save,dataStd=[],legend='upper left',ylim1=0,ylim2=0,black=True,
             sizeFig=(4,4), paper=True):
 try:
  if (paper):
      setPaperRC(sizeFig)
  else:
      setRC(sizeFig)

  # Width of bars
  width = 0.10

  data=[];
  #plt.figure(figsize=(1,1))

  for tup in data_:
   data_path = tup[0]
   values =np.atleast_2d(np.loadtxt(data_path))
   index = tup[1]
   data.append(list(values[:,index]));

  N = len(barNames)
  ind = np.arange(N)
  fig,ax = plt.subplots()

  ax.set_ylabel(yAxis1)
  ax.set_xticks(ind + width)
  ax.set_xticklabels(tuple(barNames))
  ax2 = ax.twinx()
  ax2.set_ylabel(yAxis2)

  ax.set_yscale('log')
  ax2.set_yscale('log')
  ax2.set_axisbelow(True)
  ax2.grid(b=False)
  #ax.grid(b=False)

  if (ylim1!=0):
      ax.set_ylim(top=ylim1)
  if (ylim2!=0):
      ax2.set_ylim(top=ylim2)
  i = 0
  lines = list()
  for d in datasetName:
     if (len(dataStd)==0):
         offset = i*width
         if not black:
          if (i%2==0):
           print(data[i])
           rects = ax.bar(ind + offset, data[i] , width, bottom=0,color=colours[i], label=datasetName[i])
          else:
           rects = ax2.bar(ind + offset, data[i] , width, bottom=0,color=colours[i], label=datasetName[i])
         else:
          if (i%2==0):
           rects = ax.bar(ind + offset, data[i] , width, bottom=0,color=colours[i], label=datasetName[i])
          else:
           rects = ax2.bar(ind + offset, data[i], width, bottom=0,color=colours[i], label=datasetName[i])
     else:
         if not black:
          rects = ax.bar(ind + offset, data[i], width, bottom=0,color=colours[i], yerr=tuple(dataStd[i]), label=datasetName[i])
         else:
          rects = ax.bar(ind + offset, data[i], width, bottom=0, yerr=tuple(dataStd[i]), label=datasetName[i])
     lines.append(rects)
     i = i + 1

  labs = [l.get_label() for l in lines]
  leg = ax.legend(lines,labs,loc=legend)
  frame  = leg.get_frame()
  frame.set_linewidth(0)
  frame.set_fill(None)
  plt.setp(ax.get_xticklabels(), rotation='vertical')
  #plt.title(title)
  saveGraph(save)
  plt.close()
 except Exception as e:
     print(e)
     print("Could not generate graphs")


# plot saving utility function
def saveGraph(filename_base, tight=True):
  for fmt in ['pdf','png']:
    if tight:
      plt.savefig("%s.%s" % (filename_base, fmt), format=fmt, bbox_inches='tight', pad_inches=0.01)
    else:
      plt.savefig("%s.%s" % (filename_base, fmt), format=fmt)

def setPaperRC(sizeFig):
  plt.rc('text', usetex=True)
  plt.rc('font', family='serif')
  plt.rc('font', size =12)
  plt.rc('legend', fontsize=10)
  plt.rc('figure', figsize=sizeFig)
  plt.rc('axes', linewidth=0.5)
  plt.rc('lines', linewidth=0.5)
  matplotlib.rcParams['ps.useafm'] = True
  matplotlib.rcParams['pdf.use14corefonts'] = True
  matplotlib.rcParams['text.usetex'] =True
  matplotlib.rcParams['legend.loc'] = 'best'

def setRC():
  #plt.rc('font',**{'family':'sans-serif','sans-serif':['Helvetica'],
  #             'serif':['Times'],'size':12})
  plt.rc('legend', fontsize=7)
  plt.rc('figure', figsize=(6,4))
  plt.rc('figure.subplot', left=0.10, top=0.90, bottom=0.12, right=0.95)
  plt.rc('axes', linewidth=0.5)
  plt.rc('lines', linewidth=0.5)
  matplotlib.rcParams['ps.useafm'] = True
  matplotlib.rcParams['pdf.use14corefonts'] = True
  matplotlib.rcParams['text.usetex'] =True
  matplotlib.rcParams['legend.loc'] = 'best'


def append_or_create(d, i, e):
  if not i in d:
    d[i] = [e]
  else:
    d[i].append(e)

def add_or_create(d, i, e):
  if not i in d:
    d[i] = e
  else:
    d[i] = d[i] + e

