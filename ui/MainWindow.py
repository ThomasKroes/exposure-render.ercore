
from PyQt import *

from Props.PropsWidget import *
from VolumeProperty.VolumePropertyWidget import *
from CameraWidget import *
from StatisticsWidget import *
from LogWidget import *

class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        uic.loadUi("Designer/MainWindow.ui", self)

        self._PropsWidget           = PropsWidget()
        self._VolumePropertyWidget  = VolumePropertyWidget()
        self._CameraWidget          = CameraWidget()
        self._StatisticsWidget      = StatisticsWidget()
        self._LogWidget             = LogWidget()

        self.PropsDockWidget.setWidget(self._PropsWidget)
        self.VolumePropertyDockWidget.setWidget(self._VolumePropertyWidget)
        self.CameraDockWidget.setWidget(self._CameraWidget)
        #self.StatisticsDockWidget.setWidget(self._StatisticsWidget)
        #self.LogDockWidget.setWidget(self._LogWidget)
