
from PyQt import *

class BoolEdit:
    def __init__(self, parent, name, setter, value = False):

        self._Value = False

        self._Setter = setter

        self._CheckBox = parent.findChild(QCheckBox, name + "CheckBox")

        self.SetValue(value)

        self._CheckBox.stateChanged.connect(self._OnCheckboxStateChanged)

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
            self._CheckBox.setCheckState(Qt.Unchecked)
        if self._Value == True:
            self._CheckBox.setCheckState(Qt.Checked)

    def SetVisible(self, visible):
        self._CheckBox.setVisible(visible)

    def SetEnabled(self, enabled):
        self._CheckBox.setEnabled(enabled)
