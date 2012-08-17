
from BaseEdit import *

class FloatEdit(BaseEdit):
    def __init__(self, parent, name, minimum, maximum, value = 0, precision = 1000, decimals = 3, prefix = "", suffix = ""):
        BaseEdit.__init__(self, name)

        self._Value = 0

        self._Label             = parent.findChild(QLabel, name + "Label")
        self._Slider            = parent.findChild(QSlider, name + "Slider")
        self._SpinBox           = parent.findChild(QDoubleSpinBox, name + "SpinBox")
        self._DownPushButton    = parent.findChild(QPushButton, name + "DownPushButton")
        self._UpPushButton      = parent.findChild(QPushButton, name + "UpPushButton")
        self._Precision         = precision
        self._Minimum           = 0
        self._Maximum           = 100
        self._Delta             = 1

        self.SetMinimum(minimum)
        self.SetMaximum(maximum)
        self.SetDecimals(decimals)
        self.SetPrefix(prefix)
        self.SetSuffix(suffix)
        self.SetValue(value)
        self.SetDelta(1)

        self._Slider.valueChanged.connect(self._OnSliderValueChanged)
        self._SpinBox.valueChanged.connect(self._OnSpinBoxValueChanged)

        if self._DownPushButton is not None:
            self._DownPushButton.clicked.connect(self._OnDown)

        if self._UpPushButton is not None:
            self._UpPushButton.clicked.connect(self._OnUp)

    def _OnSliderValueChanged(self, value):
        self.SetValue(value / float(self._Precision))

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
        self._Slider.setValue(self._Value * float(self._Precision))

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

    def GetMinimum(self):
        return self._Minimum

    def SetMinimum(self, minimum):
        self._Minimum = minimum
        self._Slider.setMinimum(minimum * self._Precision)
        self._SpinBox.setMinimum(minimum)

    def GetMaximum(self):
        return self._Maximum

    def SetMaximum(self, maximum):
        self._Maximum = maximum
        self._Slider.setMaximum(maximum * self._Precision)
        self._SpinBox.setMaximum(maximum)

    def SetDecimals(self, decimals):
        self._SpinBox.setDecimals(decimals)

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

