
from BaseEdit import *

class IntEdit(BaseEdit):
    def __init__(self, parent, name, minimum, maximum, value = 0, prefix = "", suffix = ""):
        BaseEdit.__init__(self, name)

        self._Value = 0

        self._Label             = parent.findChild(QLabel, name + "Label")
        self._Slider            = parent.findChild(QSlider, name + "Slider")
        self._SpinBox           = parent.findChild(QSpinBox, name + "SpinBox")
        self._DownPushButton    = parent.findChild(QPushButton, name + "DownPushButton")
        self._UpPushButton      = parent.findChild(QPushButton, name + "UpPushButton")
        self._Minimum           = 0
        self._Maximum           = 100
        self._Delta             = 1

        self.SetMinimum(minimum)
        self.SetMaximum(maximum)
        self.SetPrefix(prefix)
        self.SetSuffix(suffix)
        self.SetValue(value)

        self._Slider.valueChanged.connect(self._OnSliderValueChanged)
        self._SpinBox.valueChanged.connect(self._OnSpinBoxValueChanged)

        if self._DownPushButton is not None:
            self._DownPushButton.clicked.connect(self._OnDown)

        if self._UpPushButton is not None:
            self._UpPushButton.clicked.connect(self._OnUp)

    def _OnSliderValueChanged(self, value):
        self.SetValue(value)

    def _OnSpinBoxValueChanged(self, value):
        self.SetValue(value)

    def _OnDown(self):
        self.Decrement()

    def _OnUp(self):
        self.Increment()

    def GetValue(self):
        return self._Value

    def SetValue(self, value):
        self._Value = max(self._Minimum, min(self._Maximum, value))
        self._UpdateUI()

        if self._Setter is not None:
            self._Setter(self._Value)

    def _UpdateUI(self):
        self._SpinBox.setValue(self._Value)
        self._Slider.setValue(self._Value)

    def SetVisible(self, visible):
        if self._Label is not None:
            self._Label.setVisible(visible)
        self._Slider.setVisible(visible)
        self._SpinBox.setVisible(visible)

    def SetEnabled(self, enabled):
        if self._Label is not None:
            self._Label.setEnabled(enabled)
        self._Slider.setEnabled(enabled)
        self._SpinBox.setEnabled(enabled)

    def SetMinimum(self, minimum):
        self._Slider.setMinimum(minimum)
        self._SpinBox.setMinimum(minimum)

    def SetMaximum(self, maximum):
        self._Slider.setMaximum(maximum)
        self._SpinBox.setMaximum(maximum)

    def SetPrefix(self, prefix):
        self._SpinBox.setPrefix(prefix)

    def SetSuffix(self, suffix):
        self._SpinBox.setSuffix(suffix)

    def GetDelta(self):
        return self._Delta

    def SetDelta(self, delta):
        self._Delta = delta

    def Decrement(self):
        self.SetValue(self._Value - self._Delta)

    def Increment(self):
        self.SetValue(self._Value + self._Delta)