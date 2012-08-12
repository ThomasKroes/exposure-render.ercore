
from PyQt import *

from TransferFunctionWidget import *
from FloatEdit import *

class VolumePropertyWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('VolumePropertyWidget.ui', self)

        self._TransferFunctionWidget = TransferFunctionWidget()

        self.layout().addWidget(self._TransferFunctionWidget)

        self._SetupConnectors()

    def _SetupConnectors(self):

        self._Connectors = {}

        #self._Connectors["PrimaryStepFactor"] = FloatEdit(self, "PrimaryStepFactor", 0.1, 10)
        #self._Connectors["SecondaryStepFactor"] = FloatEdit(self, "SecondaryStepFactor", 0.1, 10)
        #self._Connectors["DensityScale"] = FloatEdit(self, "DensityScale", 0, 100)
        #self._Connectors["GradientFactor"] = FloatEdit(self, "GradientFactor", 0, 10)
