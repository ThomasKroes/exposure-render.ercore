
from BaseEdit import *

class FloatEdit(BaseEdit):
    def __init__(self, parent, name, minimum, maximum, value = 0, precision = 1000, decimals = 3, prefix = "", suffix = ""):
        BaseEdit.__init__(self, name)

        self._Value = 0

        self._AddControl("Label", parent.findChild(QLabel, name + "Label"))
        self._AddControl("Slider", parent.findChild(QSlider, name + "Slider"))
        self._AddControl("SpinBox", parent.findChild(QDoubleSpinBox, name + "SpinBox"))
        self._AddControl("DownPushButton", parent.findChild(QPushButton, name + "DownPushButton"))
        self._AddControl("UpPushButton", parent.findChild(QPushButton, name + "UpPushButton"))

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

        self._Controls["Slider"].valueChanged.connect(self._OnSliderValueChanged)
        self._Controls["SpinBox"].valueChanged.connect(self._OnSpinBoxValueChanged)

        if self._Controls.has_key("DownPushButton"):
            self._Controls["DownPushButton"].clicked.connect(self._OnDown)

        if self._Controls.has_key("UpPushButton"):
            self._Controls["UpPushButton"].clicked.connect(self._OnUp)

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
        self._Controls["SpinBox"].setValue(self._Value)
        self._Controls["Slider"].setValue(self._Value * float(self._Precision))

    def GetMinimum(self):
        return self._Minimum

    def SetMinimum(self, minimum):
        self._Minimum = minimum
        self._Controls["Slider"].setMinimum(minimum * self._Precision)
        self._Controls["SpinBox"].setMinimum(minimum)

    def GetMaximum(self):
        return self._Maximum

    def SetMaximum(self, maximum):
        self._Maximum = maximum
        self._Controls["Slider"].setMaximum(maximum * self._Precision)
        self._Controls["SpinBox"].setMaximum(maximum)

    def SetDecimals(self, decimals):
        self._Controls["SpinBox"].setDecimals(decimals)

    def SetPrefix(self, prefix):
        self._Controls["SpinBox"].setPrefix(prefix)

    def SetSuffix(self, suffix):
        self._Controls["SpinBox"].setSuffix(suffix)

    def GetDelta(self):
        return self._Delta

    def SetDelta(self, delta):
        self._Delta = delta

    def Decrement(self):
        self.SetValue(self._Value - self._Delta)

    def Increment(self):
        self.SetValue(self._Value + self._Delta)

