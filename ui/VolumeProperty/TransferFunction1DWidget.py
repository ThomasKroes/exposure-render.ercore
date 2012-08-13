
from PyQt import *

class TransferFunction1DWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi("Designer/TransferFunction1DWidget.ui", self)
