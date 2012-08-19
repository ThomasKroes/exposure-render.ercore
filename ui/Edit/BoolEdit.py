
from BaseEdit import *

class BoolEdit(BaseEdit):
    def __init__(self, parent, name, value = False):
        BaseEdit.__init__(self, name)

        self._Value = False

        self._AddControl("CheckBox", parent.findChild(QCheckBox, name + "CheckBox"))

        self.SetValue(value)

        self._Controls["CheckBox"].stateChanged.connect(self._OnCheckboxStateChanged)

    def _OnCheckboxStateChanged(self, value):
        if value == Qt.Unchecked:
            self.SetValue(False)
        if value == Qt.Checked:
            self.SetValue(True)

    def GetValue(self):
        return self._Value

    def SetValue(self, value):
        self._Value = value
        self._UpdateUI()

        if self._Setter is not None:
            self._Setter(self._Value)

    def _UpdateUI(self):
        if self._Value == False:
            self._Controls["CheckBox"].setCheckState(Qt.Unchecked)
        if self._Value == True:
            self._Controls["CheckBox"].setCheckState(Qt.Checked)