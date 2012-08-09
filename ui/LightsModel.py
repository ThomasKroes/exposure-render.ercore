
from PyQt import *

class LightsModel(QAbstractTableModel):
    def __init__(self, parent=None, *args):
        QAbstractTableModel.__init__(self, parent, *args)

        self._Lights = []

    def rowCount(self, parent):
        return len(self.arraydata)

    def columnCount(self, parent):
        return 5

    def data(self, index, role):
        if not index.isValid():
            return QVariant()
        elif role != Qt.DisplayRole:
            return QVariant()
        return QVariant(self.arraydata[index.row()][index.column()])
