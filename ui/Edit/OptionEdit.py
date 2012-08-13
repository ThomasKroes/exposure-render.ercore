
from BaseEdit import *

class OptionEdit(BaseEdit):
    def __init__(self, parent, name, value = 0):
        BaseEdit.__init__(self)

        self._Value = 0

        self._ComboBox = parent.findChild(QComboBox, name + "ComboBox")

        self.SetValue(value)

        self._ComboBox.currentIndexChanged.connect(self._OnCurrentIndexChanged)

    def _OnCurrentIndexChanged(self, value):
        self.SetValue(value)

    def GetValue(self):
        return self._Value

    def SetValue(self, value):
        self._Value = value
        self._UpdateUI()

        if self._Setter is not None:
            self._Setter(self._Value)

    def _UpdateUI(self):
        self._ComboBox.setCurrentIndex(self._Value)

    def SetVisible(self, visible):
        self._ComboBox.setVisible(visible)

    def SetEnabled(self, enabled):
        self._ComboBox.setEnabled(enabled)

