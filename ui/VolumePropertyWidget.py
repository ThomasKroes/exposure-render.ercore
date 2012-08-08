
from PyQt import *

class VolumePropertyWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('VolumePropertyWidget.ui', self)