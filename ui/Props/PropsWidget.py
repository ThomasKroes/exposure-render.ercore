
# -*- coding: utf-8 -*-

from PyQt import *
from Edit.Editable import *

class PropsWidget(QWidget, Editable):
    def __init__(self):
        QWidget.__init__(self)
        Editable.__init__(self)

        uic.loadUi("Designer/PropsWidget.ui", self)

        self.AddObjectPushButton.clicked.connect(self._OnAddObject)
        self.RemoveObjectPushButton.clicked.connect(self._OnRemoveObject)

    def _OnAddObject(self):
        self.NotifyObservers("object:add", None)
        self._ObjectsModel.AddProp("Hello")

    def _OnRemoveObject(self):
        self.NotifyObservers("object:remove", None)

    def _SetupEdits(self):
        self._Edits["AlignmentType"]        = OptionEdit(self, "AlignmentType")
        self._Edits["RelativeToCamera"]     = BoolEdit(self, "RelativeToCamera")
        self._Edits["UseCameraFocalPoint"]  = BoolEdit(self, "UseCameraFocalPoint")
        self._Edits["Axis"]                 = OptionEdit(self, "Axis")
        self._Edits["AutoFlip"]             = BoolEdit(self, "AutoFlip")
        self._Edits["Elevation"]            = FloatEdit(self, "Elevation", -90, 90)
        self._Edits["Azimuth"]              = FloatEdit(self, "Azimuth", -360, 360)
        self._Edits["Offset"]               = FloatEdit(self, "Offset", 0, 10)
        self._Edits["Position"]             = Float3Edit(self, "Position", -10, 10)
        self._Edits["Target"]               = Float3Edit(self, "Target", -10, 10)
        self._Edits["Up"]                   = Float3Edit(self, "Up", -1, 1)

        self._Edits["Elevation"].SetSuffix(u" °")
        self._Edits["Azimuth"].SetSuffix(u" °")
        self._Edits["Offset"].SetSuffix(" mm")
        self._Edits["Position"].SetSuffix(" mm")
        self._Edits["Target"].SetSuffix(" mm")
        self._Edits["Up"].SetSuffix(" mm")

        self._UpdateUI()

    def _UpdateUI(self):
        type = self._Edits["AlignmentType"].GetValue()
        self._Edits["Elevation"].SetVisible(type is 0)
        self._Edits["Azimuth"].SetVisible(type is 0)
        self._Edits["Offset"].SetVisible(type is 0)
        self._Edits["Position"].SetVisible(type is 1 or type is 2)
        self._Edits["Target"].SetVisible(type is 1)
        self._Edits["Up"].SetVisible(type is 1)

        self.AxisLabel.setVisible(type is 2)
        self.AxisComboBox.setVisible(type is 2)
        self.AutoFlipCheckBox.setVisible(type is 2)