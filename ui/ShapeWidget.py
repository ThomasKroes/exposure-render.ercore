
from PyQt import *

from SubjectMixin import *
from Float3Edit import *
from FloatEdit import *

class ShapeWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('ShapeWidget.ui', self)

        self._SetupConnectors()

    def _SetupConnectors(self):

        self._Connectors = {}

        self._Connectors["Size"] = Float3Edit(self, "Size", -10, 10)
        self._Connectors["InnerRadius"] = FloatEdit(self, "InnerRadius", 0, 10)
        self._Connectors["OuterRadius"] = FloatEdit(self, "OuterRadius", 0, 10)

        self._Connectors["Size"].SetSuffix(" mm")
        self._Connectors["InnerRadius"].SetSuffix(" mm")
        self._Connectors["OuterRadius"].SetSuffix(" mm")