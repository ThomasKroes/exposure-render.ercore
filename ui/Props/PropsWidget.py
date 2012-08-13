
from PyQt import *

from SubjectMixin import *
from Props.PropsModel import *
from Props.AlignmentWidget import AlignmentWidget

class PropsWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi("Designer/PropsWidget.ui", self)

        self._ObjectsModel  = PropsModel()

        self.PropsTableView.setModel(self._ObjectsModel)

#        self.PropsTableView.setItemDelegateForColumn(1, AlignmentToolButtonDelegate(self, self._ObjectsModel))
#        self.PropsTableView.setItemDelegateForColumn(2, ShapeToolButtonDelegate(self, self._ObjectsModel))
#        self.PropsTableView.setItemDelegateForColumn(3, EmitterToolButtonDelegate(self, self._ObjectsModel))

        self.AddObjectPushButton.clicked.connect(self._OnAddObject)
        self.RemoveObjectPushButton.clicked.connect(self._OnRemoveObject)

    def _OnAddObject(self):
        self.NotifyObservers("object:add", None)
        self._ObjectsModel.AddProp("Hello")

    def _OnRemoveObject(self):
        self.NotifyObservers("object:remove", None)
