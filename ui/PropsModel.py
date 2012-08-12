
from PyQt import *
from ExposureRender import *

class PropsModel(QAbstractTableModel):
    def __init__(self, parent=None, *args):
        QAbstractTableModel.__init__(self, parent, *args)

        self._Props = []
        self._HeaderData    = ["Name", "Alignment", "Shape", "Emitter"]

    def rowCount(self, parent):
        return len(self._Props)

    def columnCount(self, parent):
        return 4

    def flags(self, index):
        return Qt.ItemIsEnabled | Qt.ItemIsSelectable | Qt.ItemIsEditable

    def data(self, index, role):
        if not index.isValid():
            return None
        elif role != Qt.DisplayRole:
            return None

#
#        CurrentProp = self._Props[index.row()]
#
#        if index.column() == 0:
#            return CurrentProp.GetName()
#
#        if index.column() == 1:
#            return CurrentProp.GetEmitter()
#
#        if index.column() is 2:
#            if CurrentProp.GetShapeType() is 0:     return "Plane"
#            if CurrentProp.GetShapeType() is 1:     return "Disk"
#            if CurrentProp.GetShapeType() is 2:     return "Ring"
#            if CurrentProp.GetShapeType() is 3:     return "Box"
#            if CurrentProp.GetShapeType() is 4:     return "Sphere"
#            if CurrentProp.GetShapeType() is 5:     return "Cylinder"
#            if CurrentProp.GetShapeType() is 6:     return "Cone"
#            if CurrentProp.GetShapeType() is 7:     return "TriangleMesh"

        return 0

    def headerData(self, col, orientation, role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole:
            return QVariant(self._HeaderData[col])
        return QVariant()

    def AddProp(self, name):
        self.beginInsertRows(QModelIndex(), len(self._Props), len(self._Props))
        self._Props.append(vtkErObject())
        self.endInsertRows()

    def RemoveProp(self, row):
        self.beginRemoveRows(QModelIndex(), row)
        self.endRemoveRows()

    def GetProp(self, row):
        return self._Props[row]