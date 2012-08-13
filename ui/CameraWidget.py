
from PyQt import *

class CameraWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi("Designer/CameraWidget.ui", self)
