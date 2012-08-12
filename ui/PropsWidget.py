
from PyQt import *

from SubjectMixin import *
from PropsModel import *
from PropWidget import *
from AlignmentToolButton import *
from ShapeToolButton import *
from EmitterToolButton import *

class ObjectsWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('PropsWidget.ui', self)

        self._ObjectsModel  = PropsModel()

        self.PropsTableView.setModel(self._ObjectsModel)

        self.PropsTableView.setItemDelegateForColumn(1, AlignmentToolButtonDelegate(self, self._ObjectsModel))
        self.PropsTableView.setItemDelegateForColumn(2, ShapeToolButtonDelegate(self))
        self.PropsTableView.setItemDelegateForColumn(3, EmitterToolButtonDelegate(self, self._ObjectsModel))

        self._PropWidget = ObjectWidget()
        self.layout().addWidget(self._PropWidget)

        self.AddObjectPushButton.clicked.connect(self._OnAddObject)
        self.RemoveObjectPushButton.clicked.connect(self._OnRemoveObject)

    def _OnAddObject(self):
        self.NotifyObservers("object:add", None)
        self._ObjectsModel.AddProp("Hello")

    def _OnRemoveObject(self):
        self.NotifyObservers("object:remove", None)