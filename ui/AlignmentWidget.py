
# -*- coding: utf-8 -*-

from MatrixWidget import *
from FloatEdit import *
from Float3Edit import *

class AlignmentWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('AlignmentWidget.ui', self)

        #self._ElevationConnector    = Connector(self.ElevationSlider, self.ElevationSpinBox, -90, 90)
        #self._AzimuthConnector      = Connector(self.AzimuthSlider, self.AzimuthSpinBox, -360, 360)

        self._SetupConnectors()

        self.TypeComboBox.currentIndexChanged.connect(self._OnAlignmentTypeChanged)

    def _SetupConnectors(self):

        self._Connectors = {}

        self._Connectors["Elevation"] = FloatEdit(self, "Elevation", -90, 90)
        self._Connectors["Azimuth"] = FloatEdit(self, "Azimuth", -360, 360)
        self._Connectors["Offset"] = FloatEdit(self, "Offset", 0, 10)
        self._Connectors["Position"] = Float3Edit(self, "Position", -10, 10)
        self._Connectors["Target"] = Float3Edit(self, "Target", -10, 10)
        self._Connectors["Up"] = Float3Edit(self, "Up", -10, 10)

        self._Connectors["Elevation"].SetSuffix(u" °")
        self._Connectors["Azimuth"].SetSuffix(u" °")
        self._Connectors["Offset"].SetSuffix(" mm")
        self._Connectors["Position"].SetSuffix(" mm")
        self._Connectors["Target"].SetSuffix(" mm")
        self._Connectors["Up"].SetSuffix(" mm")

    def _OnAlignmentTypeChanged(self, type):

        self._Connectors["Elevation"].SetVisible(type is 0)
        self._Connectors["Azimuth"].SetVisible(type is 0)
        self._Connectors["Offset"].SetVisible(type is 0)
        self._Connectors["Position"].SetVisible(type is 1 or type is 2)
        self._Connectors["Target"].SetVisible(type is 1)
        self._Connectors["Up"].SetVisible(type is 1)

        self.AlignToLabel.setVisible(type is 2)
        self.AlignToComboBox.setVisible(type is 2)
        self.AutoFlipCheckBox.setVisible(type is 2)