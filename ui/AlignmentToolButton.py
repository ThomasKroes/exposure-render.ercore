
from PyQt import *

from AlignmentWidget import *

class AlignmentToolButton(QToolButton):
    def __init__(self, owner, prop):
        QToolButton.__init__(self, owner)

        self.setPopupMode(QToolButton.InstantPopup)
        self.setToolButtonStyle(Qt.ToolButtonTextOnly)
        self.setText("...")
        self.setMenu(QMenu(self))

        self._AlignmentWidget = AlignmentWidget(prop)

        Action = QWidgetAction(self)
        Action.setDefaultWidget(self._AlignmentWidget)

        self.menu().addAction(Action)

class AlignmentToolButtonDelegate(QItemDelegate):
    def __init__(self, owner, model):
        QItemDelegate.__init__(self, owner)

        self._Model = model

    def createEditor(self, parent, option, index):
        return AlignmentToolButton(parent, self._Model.GetProp(index.row()))
