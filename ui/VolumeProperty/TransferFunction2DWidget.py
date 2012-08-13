
from PyQt import *

class TransferFunction2DWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi("Designer/TransferFunction2DWidget.ui", self)
