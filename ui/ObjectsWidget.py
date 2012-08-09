
from PyQt import *

from SubjectMixin import *
from ObjectsModel import *
from ObjectWidget import *

class ObjectsWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('ObjectsWidget.ui', self)

        self._ObjectsModel  = ObjectsModel()
        self.ObjectsTableView.setModel(self._ObjectsModel)

        self._ObjectWidget = ObjectWidget()
        self.layout().addWidget(self._ObjectWidget)

        self.AddObjectPushButton.clicked.connect(self._OnAddObject)
        self.RemoveObjectPushButton.clicked.connect(self._OnRemoveObject)

    def _OnAddObject(self):
        self.NotifyObservers("object:add", None)
        self._ObjectsModel.AddObject("Hello")

    def _OnRemoveObject(self):
        self.NotifyObservers("object:remove", None)