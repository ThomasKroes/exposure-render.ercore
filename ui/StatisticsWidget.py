
from PyQt import *

class StatisticsWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        uic.loadUi("Designer/StatisticsWidget.ui", self)
