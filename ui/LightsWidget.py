
from PyQt import *

class LightsWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi('LightsWidget.ui', self)
