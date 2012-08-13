
from PyQt import *

class LogWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi("Designer/LogWidget.ui", self)
