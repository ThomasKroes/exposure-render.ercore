
from PyQt import *

from Props.EmitterWidget import *

class EmitterToolButton(QToolButton):
    def __init__(self, owner, prop):
        QToolButton.__init__(self, owner)

        self.setPopupMode(QToolButton.InstantPopup)
        self.setToolButtonStyle(Qt.ToolButtonTextOnly)
        self.setText("...")
        self.setMenu(QMenu(self))

        self._EmitterWidget = EmitterWidget(prop)

        Action = QWidgetAction(self)
        Action.setDefaultWidget(self._EmitterWidget)

        self.menu().addAction(Action)

class EmitterToolButtonDelegate(QItemDelegate):
    def __init__(self, owner, model):
        QItemDelegate.__init__(self, owner)

        self._Model = model

    def createEditor(self, parent, option, index):
        return EmitterToolButton(parent, self._Model)
