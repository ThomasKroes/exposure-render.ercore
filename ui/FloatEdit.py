
from PyQt import *

class FloatEdit:
    def __init__(self, parent, name, minimum, maximum, precision = 1000, decimals = 3, prefix = "", suffix = ""):
        self._Label     = parent.findChild(QLabel, name + "Label")
        self._Slider    = parent.findChild(QSlider, name + "Slider")
        self._SpinBox   = parent.findChild(QDoubleSpinBox, name + "SpinBox")
        self._Precision = precision

        self.SetMinimum(minimum)
        self.SetMaximum(maximum)
        self.SetDecimals(decimals)
        self.SetPrefix(prefix)
        self.SetSuffix(suffix)

        self._Slider.valueChanged.connect(self._OnSliderValueChanged)
        self._SpinBox.valueChanged.connect(self._OnSpinBoxValueChanged)

    def _OnSliderValueChanged(self, value):
        self._SpinBox.setValue(value / float(self._Precision))

    def _OnSpinBoxValueChanged(self, value):
        self._Slider.setValue(value * float(self._Precision))

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
        self._Slider.setMinimum(minimum * self._Precision)
        self._SpinBox.setMinimum(minimum)

    def SetMaximum(self, maximum):
        self._Slider.setMaximum(maximum * self._Precision)
        self._SpinBox.setMaximum(maximum)

    def SetDecimals(self, decimals):
        self._SpinBox.setDecimals(decimals)

    def SetPrefix(self, prefix):
        self._SpinBox.setPrefix(prefix)

    def SetSuffix(self, suffix):
        self._SpinBox.setSuffix(suffix)