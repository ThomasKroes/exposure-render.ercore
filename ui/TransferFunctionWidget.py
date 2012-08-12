
from PyQt import *

from TransferFunction1DWidget import *
from TransferFunction2DWidget import *

class TransferFunctionWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('TransferFunctionWidget.ui', self)

        self.layout().setContentsMargins(0, 0, 0, 0)

        self._TransferFunction1DWidget = TransferFunction1DWidget()
        self._TransferFunction2DWidget = TransferFunction2DWidget()

        self.TransferFunctionTabWidget.addTab(self._TransferFunction1DWidget, "1D")
        self.TransferFunctionTabWidget.addTab(self._TransferFunction2DWidget, "2D")
