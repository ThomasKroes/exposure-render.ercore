
from PyQt import *

from TransferFunctionWidget import *

class VolumePropertyWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('VolumePropertyWidget.ui', self)

        self._TransferFunctionWidget = TransferFunctionWidget()

        self.layout().addWidget(self._TransferFunctionWidget)
