
from PyQt import *

from ObjectsWidget import *
from VolumePropertyWidget import *
from CameraWidget import *
from StatisticsWidget import *
from LogWidget import *

class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        uic.loadUi('MainWindow.ui', self)

        self._ObjectsWidget         = ObjectsWidget()
        self._VolumePropertyWidget  = VolumePropertyWidget()
        self._CameraWidget          = CameraWidget()
        self._StatisticsWidget      = StatisticsWidget()
        self._LogWidget             = LogWidget()

        self.ObjectsDockWidget.setWidget(self._ObjectsWidget)
        self.VolumePropertyDockWidget.setWidget(self._VolumePropertyWidget)
        self.CameraDockWidget.setWidget(self._CameraWidget)
        self.StatisticsDockWidget.setWidget(self._StatisticsWidget)
        self.LogDockWidget.setWidget(self._LogWidget)

        #self._SimulationCore.BoneGeometry().AddObserver('change:light', self._OnBoneGeometryChange)
