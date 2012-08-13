
# -*- coding: utf-8 -*-

from MatrixWidget import *
from Edit.Editable import *
from Edit.Edits import *

class AlignmentWidget(QWidget, SubjectMixin, Editable):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi("Designer/AlignmentWidget.ui", self)

        Editable.__init__(self)

        self.TypeComboBox.currentIndexChanged.connect(self._OnAlignmentTypeChanged)

    def sizeHint(self):
        return QSize(1, 1)

    def _SetupEdits(self):
        self._Edits["Type"] = OptionEdit(self, "Type")
        self._Edits["RelativeToCamera"] = BoolEdit(self, "RelativeToCamera")
        self._Edits["UseCameraFocalPoint"] = BoolEdit(self, "UseCameraFocalPoint")
        self._Edits["Axis"] = OptionEdit(self, "Axis")
        self._Edits["AutoFlip"] = BoolEdit(self, "AutoFlip")
        self._Edits["Elevation"] = FloatEdit(self, "Elevation", -90, 90)
        self._Edits["Azimuth"] = FloatEdit(self, "Azimuth", -360, 360)
        self._Edits["Offset"] = FloatEdit(self, "Offset", 0, 10)
        self._Edits["Position"] = Float3Edit(self, "Position", -10, 10)
        self._Edits["Target"] = Float3Edit(self, "Target", -10, 10)
        self._Edits["Up"] = Float3Edit(self, "Up", -1, 1)

        self._Edits["Elevation"].SetSuffix(u" °")
        self._Edits["Azimuth"].SetSuffix(u" °")
        self._Edits["Offset"].SetSuffix(" mm")
        self._Edits["Position"].SetSuffix(" mm")
        self._Edits["Target"].SetSuffix(" mm")
        self._Edits["Up"].SetSuffix(" mm")

        self._UpdateUI()

    def _OnAlignmentTypeChanged(self, type):
        self._UpdateUI()

    def _UpdateUI(self):
        type = self._Edits["Type"].GetValue()
        self._Edits["Elevation"].SetVisible(type is 0)
        self._Edits["Azimuth"].SetVisible(type is 0)
        self._Edits["Offset"].SetVisible(type is 0)
        self._Edits["Position"].SetVisible(type is 1 or type is 2)
        self._Edits["Target"].SetVisible(type is 1)
        self._Edits["Up"].SetVisible(type is 1)

        self.AxisLabel.setVisible(type is 2)
        self.AxisComboBox.setVisible(type is 2)
        self.AutoFlipCheckBox.setVisible(type is 2)

    def SetProp(self, prop):
        self._Edits["Type"].Connect(prop.SetAlignmentType, prop.GetAlignmentType())
        self._Edits["RelativeToCamera"].Connect(prop.SetRelativeToCamera, prop.GetRelativeToCamera())
        self._Edits["UseCameraFocalPoint"].Connect(prop.SetUseCameraFocalPoint, prop.GetUseCameraFocalPoint())
        self._Edits["Axis"].Connect(prop.SetAxis, prop.GetAxis())
        self._Edits["AutoFlip"].Connect(prop.SetAutoFlip, prop.GetAutoFlip())
        self._Edits["Elevation"].Connect(prop.SetElevation, prop.GetElevation())
        self._Edits["Azimuth"].Connect(prop.SetAzimuth, prop.GetAzimuth())
        self._Edits["Offset"].Connect(prop.SetOffset, prop.GetOffset())
        self._Edits["Position"].Connect(prop.SetPosition, prop.GetPosition())
        self._Edits["Target"].Connect(prop.SetTarget, prop.GetTarget())
        self._Edits["Up"].Connect(prop.SetUp, prop.GetUp())