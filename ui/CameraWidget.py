
from PyQt import *

class CameraWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('CameraWidget.ui', self)
