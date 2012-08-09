
from PyQt import *

from TransferFunction1DWidget import *
from TransferFunction2DWidget import *

class TransferFunctionWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('TransferFunctionWidget.ui', self)

        self._TransferFunction1DWidget = TransferFunction1DWidget()
        self._TransferFunction2DWidget = TransferFunction2DWidget()

        self.TransferFunctionTabWidget.addTab(self._TransferFunction1DWidget, "1D Transfer Function")
        self.TransferFunctionTabWidget.addTab(self._TransferFunction2DWidget, "2D Transfer Function")
        #self.layout().addWidget(self._TransferFunction2DWidget)