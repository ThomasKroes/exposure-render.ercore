
# -*- coding: utf-8 -*-

from MatrixWidget import *
from Edit import *

class AlignmentWidget(QWidget, SubjectMixin):
    def __init__(self, prop):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        self._Prop = prop

        uic.loadUi('AlignmentWidget.ui', self)

        self._SetupConnectors()

        self.TypeComboBox.currentIndexChanged.connect(self._OnAlignmentTypeChanged)

    def _SetupConnectors(self):

        self._Connectors = {}

        self._Connectors["Type"] = OptionEdit(self, "Type", self._Prop.SetAlignmentType, self._Prop.GetAlignmentType())
        self._Connectors["RelativeToCamera"] = BoolEdit(self, "RelativeToCamera", self._Prop.SetRelativeToCamera, self._Prop.GetRelativeToCamera())
        self._Connectors["UseCameraFocalPoint"] = BoolEdit(self, "UseCameraFocalPoint", self._Prop.SetUseCameraFocalPoint, self._Prop.GetUseCameraFocalPoint())
        self._Connectors["Axis"] = OptionEdit(self, "Axis", self._Prop.SetAxis, self._Prop.GetAxis())
        self._Connectors["AutoFlip"] = BoolEdit(self, "AutoFlip", self._Prop.SetAutoFlip, self._Prop.GetAutoFlip())
        self._Connectors["Elevation"] = FloatEdit(self, "Elevation", -90, 90, self._Prop.SetElevation, self._Prop.GetElevation())
        self._Connectors["Azimuth"] = FloatEdit(self, "Azimuth", -360, 360, self._Prop.SetAzimuth, self._Prop.GetAzimuth())
        self._Connectors["Offset"] = FloatEdit(self, "Offset", 0, 10, self._Prop.SetOffset, self._Prop.GetOffset())
        self._Connectors["Position"] = Float3Edit(self, "Position", -10, 10, self._Prop.SetPosition, self._Prop.GetPosition())
        self._Connectors["Target"] = Float3Edit(self, "Target", -10, 10, self._Prop.SetTarget, self._Prop.GetTarget())
        self._Connectors["Up"] = Float3Edit(self, "Up", -1, 1, self._Prop.SetUp, self._Prop.GetUp())

        self._Connectors["Elevation"].SetSuffix(u" °")
        self._Connectors["Azimuth"].SetSuffix(u" °")
        self._Connectors["Offset"].SetSuffix(" mm")
        self._Connectors["Position"].SetSuffix(" mm")
        self._Connectors["Target"].SetSuffix(" mm")
        self._Connectors["Up"].SetSuffix(" mm")

        self._UpdateUI()

    def _OnAlignmentTypeChanged(self, type):
        self._UpdateUI()

    def _UpdateUI(self):
        type = self._Connectors["Type"].GetValue()
        self._Connectors["Elevation"].SetVisible(type is 0)
        self._Connectors["Azimuth"].SetVisible(type is 0)
        self._Connectors["Offset"].SetVisible(type is 0)
        self._Connectors["Position"].SetVisible(type is 1 or type is 2)
        self._Connectors["Target"].SetVisible(type is 1)
        self._Connectors["Up"].SetVisible(type is 1)

        self.AxisLabel.setVisible(type is 2)
        self.AxisComboBox.setVisible(type is 2)
        self.AutoFlipCheckBox.setVisible(type is 2)
