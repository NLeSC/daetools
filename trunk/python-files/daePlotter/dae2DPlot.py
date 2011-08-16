"""********************************************************************************
                             dae2DPlot.py
                 DAE Tools: pyDAE module, www.daetools.com
                 Copyright (C) Dragan Nikolic, 2010
***********************************************************************************
DAE Tools is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License version 3 as published by the Free Software 
Foundation. DAE Tools is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with the
DAE Tools software; if not, see <http://www.gnu.org/licenses/>.
********************************************************************************"""
import sys, numpy
from daetools.pyDAE import *
from daeChooseVariable import daeChooseVariable, daeTableDialog
from daePlotOptions import *
from PyQt4 import QtCore, QtGui
import matplotlib
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar


class daePlot2dDefaults:
    def __init__(self, color='black', linewidth=0.5, linestyle='solid', marker='o', markersize=6, markerfacecolor='black', markeredgecolor='black'):
        self.color = color
        self.linewidth = linewidth
        self.linestyle = linestyle
        self.marker = marker
        self.markersize = markersize
        self.markerfacecolor = markerfacecolor
        self.markeredgecolor = markeredgecolor

class dae2DPlot(QtGui.QDialog):
    plotDefaults = [daePlot2dDefaults('black', 0.5, 'solid', 'o', 6, 'black', 'black'),
                    daePlot2dDefaults('blue',  0.5, 'solid', 's', 6, 'blue',  'black'),
                    daePlot2dDefaults('red',   0.5, 'solid', '^', 6, 'red',   'black'),
                    daePlot2dDefaults('green', 0.5, 'solid', 'p', 6, 'green', 'black'),
                    daePlot2dDefaults('c',     0.5, 'solid', 'h', 6, 'c',     'black'),
                    daePlot2dDefaults('m',     0.5, 'solid', '*', 6, 'm',     'black'),
                    daePlot2dDefaults('k',     0.5, 'solid', 'd', 6, 'k',     'black'),
                    daePlot2dDefaults('y',     0.5, 'solid', 'x', 6, 'y',     'black'),
                    
                    daePlot2dDefaults('black', 0.5, 'dashed', 'o', 6, 'black', 'black'),
                    daePlot2dDefaults('blue',  0.5, 'dashed', 's', 6, 'blue',  'black'),
                    daePlot2dDefaults('red',   0.5, 'dashed', '^', 6, 'red',   'black'),
                    daePlot2dDefaults('green', 0.5, 'dashed', 'p', 6, 'green', 'black'),
                    daePlot2dDefaults('c',     0.5, 'dashed', 'h', 6, 'c',     'black'),
                    daePlot2dDefaults('m',     0.5, 'dashed', '*', 6, 'm',     'black'),
                    daePlot2dDefaults('k',     0.5, 'dashed', 'd', 6, 'k',     'black'),
                    daePlot2dDefaults('y',     0.5, 'dashed', 'x', 6, 'y',     'black'),
                    
                    daePlot2dDefaults('black', 0.5, 'dotted', 'o', 6, 'black', 'black'),
                    daePlot2dDefaults('blue',  0.5, 'dotted', 's', 6, 'blue',  'black'),
                    daePlot2dDefaults('red',   0.5, 'dotted', '^', 6, 'red',   'black'),
                    daePlot2dDefaults('green', 0.5, 'dotted', 'p', 6, 'green', 'black'),
                    daePlot2dDefaults('c',     0.5, 'dotted', 'h', 6, 'c',     'black'),
                    daePlot2dDefaults('m',     0.5, 'dotted', '*', 6, 'm',     'black'),
                    daePlot2dDefaults('k',     0.5, 'dotted', 'd', 6, 'k',     'black'),
                    daePlot2dDefaults('y',     0.5, 'dotted', 'x', 6, 'y',     'black') ]
                    
    def __init__(self, parent, tcpipServer):
        QtGui.QDialog.__init__(self, parent, QtCore.Qt.Window)
        
        self.tcpipServer = tcpipServer
        
        self.legendOn = True
        self.gridOn   = True
        
        self.setWindowTitle("2D plot")
        self.setWindowIcon(QtGui.QIcon('images/line-chart.png'))

        exit = QtGui.QAction(QtGui.QIcon('images/close.png'), 'Exit', self)
        exit.setShortcut('Ctrl+Q')
        exit.setStatusTip('Exit application')
        self.connect(exit, QtCore.SIGNAL('triggered()'), self.close)

        properties = QtGui.QAction(QtGui.QIcon('images/preferences.png'), 'Options', self)
        properties.setShortcut('Ctrl+P')
        properties.setStatusTip('Options')
        self.connect(properties, QtCore.SIGNAL('triggered()'), self.slotProperties)

        grid = QtGui.QAction(QtGui.QIcon('images/grid.png'), 'Grid on/off', self)
        grid.setShortcut('Ctrl+G')
        grid.setStatusTip('Grid on/off')
        self.connect(grid, QtCore.SIGNAL('triggered()'), self.slotToggleGrid)

        legend = QtGui.QAction(QtGui.QIcon('images/legend.png'), 'Legend on/off', self)
        legend.setShortcut('Ctrl+L')
        legend.setStatusTip('Legend on/off')
        self.connect(legend, QtCore.SIGNAL('triggered()'), self.slotToggleLegend)

        viewdata = QtGui.QAction(QtGui.QIcon('images/data.png'), 'View tabular data', self)
        viewdata.setShortcut('Ctrl+D')
        viewdata.setStatusTip('View tabular data')
        self.connect(viewdata, QtCore.SIGNAL('triggered()'), self.slotViewTabularData)

        csv = QtGui.QAction(QtGui.QIcon('images/csv.png'), 'Export CSV', self)
        csv.setShortcut('Ctrl+S')
        csv.setStatusTip('Export CSV')
        self.connect(csv, QtCore.SIGNAL('triggered()'), self.slotExportCSV)

        remove_line = QtGui.QAction(QtGui.QIcon('images/remove.png'), 'Remove line', self)
        remove_line.setShortcut('Ctrl+R')
        remove_line.setStatusTip('Remove line')
        self.connect(remove_line, QtCore.SIGNAL('triggered()'), self.slotRemoveLine)

        new_line = QtGui.QAction(QtGui.QIcon('images/add.png'), 'Add line', self)
        new_line.setShortcut('Ctrl+A')
        new_line.setStatusTip('Add line')
        self.connect(new_line, QtCore.SIGNAL('triggered()'), self.newCurve)

        self.toolbar_widget = QtGui.QWidget(self)
        layoutToolbar = QtGui.QVBoxLayout(self.toolbar_widget)
        layoutToolbar.setContentsMargins(0,0,0,0)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        self.toolbar_widget.setSizePolicy(sizePolicy)

        layoutPlot = QtGui.QVBoxLayout(self)
        layoutPlot.setContentsMargins(2,2,2,2)
        self.figure = Figure((6.0, 4.0), dpi=100, facecolor='white')#"#E5E5E5")
        self.canvas = FigureCanvas(self.figure)
        self.canvas.setParent(self)        
        self.canvas.axes = self.figure.add_subplot(111)
        
        self.mpl_toolbar = NavigationToolbar(self.canvas, self.toolbar_widget, False)
        
        #self.mpl_toolbar.addSeparator()
        self.mpl_toolbar.addAction(csv)
        self.mpl_toolbar.addAction(viewdata)
        self.mpl_toolbar.addSeparator()
        self.mpl_toolbar.addAction(grid)
        self.mpl_toolbar.addAction(legend)
        self.mpl_toolbar.addSeparator()
        self.mpl_toolbar.addAction(new_line)
        self.mpl_toolbar.addAction(remove_line)
        self.mpl_toolbar.addSeparator()
        self.mpl_toolbar.addAction(properties)
        #self.mpl_toolbar.addSeparator()
        #self.mpl_toolbar.addAction(exit)
        
        self.fp8  = matplotlib.font_manager.FontProperties(family='sans-serif', style='normal', variant='normal', weight='normal', size=8)
        self.fp9  = matplotlib.font_manager.FontProperties(family='sans-serif', style='normal', variant='normal', weight='normal', size=9)
        self.fp11 = matplotlib.font_manager.FontProperties(family='sans-serif', style='normal', variant='normal', weight='normal', size=11)
        
        for xlabel in self.canvas.axes.get_xticklabels():
            xlabel.set_fontproperties(self.fp9)
        for ylabel in self.canvas.axes.get_yticklabels():
            ylabel.set_fontproperties(self.fp9)

        layoutToolbar.addWidget(self.mpl_toolbar)
        layoutPlot.addWidget(self.canvas)
        layoutPlot.addWidget(self.toolbar_widget)

    #@QtCore.pyqtSlot()
    def slotProperties(self):
        figure_edit(self.canvas, self)

    #@QtCore.pyqtSlot()
    def slotToggleLegend(self):
        self.legendOn = not self.legendOn
        self.reformatPlot()    
        
    #@QtCore.pyqtSlot()
    def slotToggleGrid(self):
        self.gridOn = not self.gridOn
        self.reformatPlot()    

    #@QtCore.pyqtSlot()
    def slotExportCSV(self):
        strInitialFilename = QtCore.QDir.current().path()
        strInitialFilename += "/untitled.csv";
        strExt = "Comma separated files (*.csv)"
        strCaption = "Save file"
        fileName = QtGui.QFileDialog.getSaveFileName(self, strCaption, strInitialFilename, strExt)
        if(fileName.isEmpty()):
            return

        datafile = open(str(fileName), 'w')
        lines = self.canvas.axes.get_lines()

        for line in lines:
            xlabel = self.canvas.axes.get_xlabel()
            ylabel = line.get_label()
            x = line.get_xdata()
            y = line.get_ydata()
            datafile.write('\"' + xlabel + '\",\"' + ylabel + '\"\n' )
            for i in range(0, len(x)):
                datafile.write(str(x[i]) + ',' + str(y[i]) + '\n')
            datafile.write('\n')

    #@QtCore.pyqtSlot()
    def slotViewTabularData(self):
        lines = self.canvas.axes.get_lines()

        tableDialog = daeTableDialog(self)
        tableDialog.setWindowTitle('Raw data')
        table = tableDialog.ui.tableWidget
        nr = 0
        ncol = len(lines)
        for line in lines:
            n = len(line.get_xdata())
            if nr < n:
                nr = n
                
        xlabel = self.canvas.axes.get_xlabel()
        table.setRowCount(nr)
        table.setColumnCount(ncol)
        horHeader = []
        verHeader = []
        for i, line in enumerate(lines):
            xlabel = self.canvas.axes.get_xlabel()
            ylabel = line.get_label()
            x = line.get_xdata()
            y = line.get_ydata()
            horHeader.append(ylabel)
            for k in range(0, len(x)):
                newItem = QtGui.QTableWidgetItem(str(y[k]))
                table.setItem(k, i, newItem)
            for k in range(0, len(x)):
                verHeader.append(str(x[k]))
            
        table.setHorizontalHeaderLabels(horHeader)
        table.setVerticalHeaderLabels(verHeader)
        table.resizeRowsToContents()
        tableDialog.exec_()

    #@QtCore.pyqtSlot()
    def slotRemoveLine(self):
        lines = self.canvas.axes.get_lines()
        items = []
        for line in lines:
            label = line.get_label()
            items.append(label)

        nameToRemove, ok = QtGui.QInputDialog.getItem(self, "Choose line to remove", "Lines:", items, 0, False)
        if ok:
            for i, line in enumerate(lines):
                label = line.get_label()
                if label == str(nameToRemove):
                    self.canvas.axes.lines.pop(i)
                    self.reformatPlot()
                    return

    #@QtCore.pyqtSlot()
    def newCurve(self):
        NoOfProcesses = self.tcpipServer.NumberOfProcesses
        processes = []
        for i in range(0, NoOfProcesses):
            processes.append(self.tcpipServer.GetProcess(i))
        cv = daeChooseVariable(processes, daeChooseVariable.plot2D)
        cv.setWindowTitle('Choose variable for 2D plot')
        if cv.exec_() != QtGui.QDialog.Accepted:
            return False
            
        domainIndexes, xAxisLabel, yAxisLabel, xPoints, yPoints = cv.getPlot2DData()

        domains = "("
        for i in range(0, len(domainIndexes)):
            if i != 0:
                domains += ", "
            domains += domainIndexes[i]
        domains += ")"

        self.addLine(xAxisLabel, yAxisLabel, xPoints, yPoints, domains)
        self.setWindowTitle(yAxisLabel + domains)

        return True
        
    def addLine(self, xAxisLabel, yAxisLabel, xPoints, yPoints, domains):
        no_lines = len(self.canvas.axes.get_lines())
        n = no_lines % len(dae2DPlot.plotDefaults)
        pd = dae2DPlot.plotDefaults[n]
        
        self.canvas.axes.plot(xPoints, yPoints, label=yAxisLabel+domains, color=pd.color, linewidth=pd.linewidth, \
                              linestyle=pd.linestyle, marker=pd.marker, markersize=pd.markersize, markerfacecolor=pd.markerfacecolor, markeredgecolor=pd.markeredgecolor)

        if no_lines == 0:
            self.canvas.axes.set_xlabel(xAxisLabel, fontproperties=self.fp11)
            self.canvas.axes.set_ylabel(yAxisLabel, fontproperties=self.fp11)
        
        self.reformatPlot()    

    def reformatPlot(self):
        lines = self.canvas.axes.get_lines()
        xmin = 1e20
        xmax = -1e20
        ymin = 1e20
        ymax = -1e20
        for line in lines:
            if min(line.get_xdata()) < xmin:
                xmin = min(line.get_xdata())
            if max(line.get_xdata()) > xmax:
                xmax = max(line.get_xdata())

            if min(line.get_ydata()) < ymin:
                ymin = min(line.get_ydata())
            if max(line.get_ydata()) > ymax:
                ymax = max(line.get_ydata())
        
        dx = (xmax - xmin) * 0.05
        dy = (ymax - ymin) * 0.05
        xmin -= dx
        xmax += dx
        ymin -= dy
        ymax += dy

        self.canvas.axes.set_xlim(xmin, xmax)
        self.canvas.axes.set_ylim(ymin, ymax)

        self.canvas.axes.grid(self.gridOn)
            
        if self.legendOn:
            self.canvas.axes.legend(loc = 0, prop=self.fp8, numpoints = 1, fancybox=True)
        else:
            self.canvas.axes.legend_ = None

        self.canvas.draw()

