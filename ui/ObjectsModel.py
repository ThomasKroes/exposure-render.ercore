
from PyQt import *
from ExposureRender import *

class ObjectsModel(QAbstractTableModel):
    def __init__(self, parent=None, *args):
        QAbstractTableModel.__init__(self, parent, *args)

        self._Objects = [ vtkErObject(), vtkErObject() ]
        self._HeaderData    = ["Name", "Emitter", "Shape type", "Emission type"]

    def rowCount(self, parent):
        return len(self._Objects)

    def columnCount(self, parent):
        return 4

    def data(self, index, role):
        if not index.isValid():
            return None
        elif role != Qt.DisplayRole:
            return None

        if index.column() == 0:
            return "Hello world"

        if index.column() == 1:
            return True


        return 0

    def headerData(self, col, orientation, role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole:
            return QVariant(self._HeaderData[col])
        return QVariant()

    def AddObject(self, name):
        self._Objects.append("sda")


