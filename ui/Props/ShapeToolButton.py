
from PyQt import *

from Props.ShapeWidget import *

class ShapeToolButton(QToolButton):
    def __init__(self, owner, prop):
        QToolButton.__init__(self, owner)

        self.setPopupMode(QToolButton.InstantPopup)
        self.setToolButtonStyle(Qt.ToolButtonTextOnly)
        self.setText("...")
        self.setMenu(QMenu(self))

        self._ShapeWidget = ShapeWidget(prop)

        Action = QWidgetAction(self)
        Action.setDefaultWidget(self._ShapeWidget)

        self.menu().addAction(Action)

class ShapeToolButtonDelegate(QItemDelegate):
    def __init__(self, owner, model):
        QItemDelegate.__init__(self, owner)

        self._Model = model

    def createEditor(self, parent, option, index):
        return ShapeToolButton(parent, self._Model.GetProp(index.row()))
