
from PyQt import *

from SubjectMixin import *

class ShapeWidget(QWidget, SubjectMixin):
    def __init__(self, prop):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        self._Prop = prop

        uic.loadUi("Designer/ShapeWidget.ui", self)

        self._SetupConnectors()

        self.TypeComboBox.currentIndexChanged.connect(self._OnShapeTypeChanged)

    def _SetupConnectors(self):

        self._Connectors = {}

        self._Connectors["Type"] = OptionEdit(self, "Type", self._Prop.SetShapeType, self._Prop.GetShapeType())
        self._Connectors["OneSided"] = BoolEdit(self, "OneSided", self._Prop.SetOneSided, self._Prop.GetOneSided())
        self._Connectors["Size"] = Float3Edit(self, "Size", 0, 10, self._Prop.SetSize, self._Prop.GetSize())
        self._Connectors["InnerRadius"] = FloatEdit(self, "InnerRadius", 0, 10, self._Prop.SetInnerRadius, self._Prop.GetInnerRadius())
        self._Connectors["OuterRadius"] = FloatEdit(self, "OuterRadius", 0, 10, self._Prop.SetOuterRadius, self._Prop.GetOuterRadius())

        self._Connectors["Size"].SetSuffix(" mm")
        self._Connectors["InnerRadius"].SetSuffix(" mm")
        self._Connectors["OuterRadius"].SetSuffix(" mm")

        self._UpdateUI()

    def _OnShapeTypeChanged(self, type):
        self._UpdateUI()

    def _UpdateUI(self):
        ShapeType = self._Connectors["Type"].GetValue()

        self._Connectors["Size"].SetVisible(ShapeType is 0 or ShapeType is 3)
        self._Connectors["Size"].SetZVisible(ShapeType is 3)
        self._Connectors["InnerRadius"].SetVisible(ShapeType is 2 or ShapeType is 6)
        self._Connectors["OuterRadius"].SetVisible(ShapeType is 1 or ShapeType is 2 or ShapeType is 4 or ShapeType is 5 or ShapeType is 6)

        self.SizeLockCheckBox.setVisible(ShapeType is 0 or ShapeType is 3)
        self.OneSidedCheckBox.setVisible(ShapeType < 3)

        self.layout().update()