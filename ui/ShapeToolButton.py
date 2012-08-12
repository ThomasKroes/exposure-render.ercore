
from PyQt import *

from ShapeWidget import *

class ShapeToolButton(QToolButton):
    def __init__(self, owner):
        QToolButton.__init__(self, owner)

        self.setPopupMode(QToolButton.InstantPopup)
        self.setToolButtonStyle(Qt.ToolButtonTextOnly)
        self.setText("...")
        self.setMenu(QMenu(self))

        self._ShapeWidget = ShapeWidget()

        Action = QWidgetAction(self)
        Action.setDefaultWidget(self._ShapeWidget)

        self.menu().addAction(Action)

class ShapeToolButtonDelegate(QItemDelegate):
    def __init__(self, owner):
        QItemDelegate.__init__(self, owner)

    def createEditor(self, parent, option, index):
        return ShapeToolButton(parent)
