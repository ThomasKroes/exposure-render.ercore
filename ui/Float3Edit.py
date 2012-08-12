
from PyQt import *

class Float3Edit:
    def __init__(self, parent, name, minimum, maximum, setter, value = [0, 0, 0], precision = 1000, decimals = 3, prefix = "", suffix = ""):

        self._Setter = setter
        self._Value = [0, 0, 0]

        self._Label = parent.findChild(QLabel, name + "Label")

        self._Lock = False

        self._SizeLockCheckBox = parent.findChild(QCheckBox, name + "LockCheckBox")

        self._Sliders = {}

        self._Sliders["X"] = parent.findChild(QSlider, name + "XSlider")
        self._Sliders["Y"] = parent.findChild(QSlider, name + "YSlider")
        self._Sliders["Z"] = parent.findChild(QSlider, name + "ZSlider")

        self._SpinBoxes = {}

        self._SpinBoxes["X"] = parent.findChild(QDoubleSpinBox, name + "XSpinBox")
        self._SpinBoxes["Y"] = parent.findChild(QDoubleSpinBox, name + "YSpinBox")
        self._SpinBoxes["Z"] = parent.findChild(QDoubleSpinBox, name + "ZSpinBox")

        self._Precision = precision

        self.SetMinimum(minimum)
        self.SetMaximum(maximum)
        self.SetDecimals(decimals)
        self.SetPrefix(prefix)
        self.SetSuffix(suffix)
        self.SetValue(value)

        self._Sliders["X"].valueChanged.connect(self._OnSliderXValueChanged)
        self._Sliders["Y"].valueChanged.connect(self._OnSliderYValueChanged)
        self._Sliders["Z"].valueChanged.connect(self._OnSliderZValueChanged)

        self._SpinBoxes["X"].valueChanged.connect(self._OnSpinBoxXValueChanged)
        self._SpinBoxes["Y"].valueChanged.connect(self._OnSpinBoxYValueChanged)
        self._SpinBoxes["Z"].valueChanged.connect(self._OnSpinBoxZValueChanged)

        if self._SizeLockCheckBox is not None:
            self._SizeLockCheckBox.stateChanged.connect(self._OnLock)

    def _OnSliderXValueChanged(self, value):
        Value = value / float(self._Precision)

        if self._Lock is True:
            self.SetValue([Value, Value, Value])
        else:
            self.SetValueDim("X", Value)

    def _OnSliderYValueChanged(self, value):
        Value = value / float(self._Precision)

        if self._Lock is True:
            self.SetValue([Value, Value, Value])
        else:
            self.SetValueDim("Y", Value)

    def _OnSliderZValueChanged(self, value):
        Value = value / float(self._Precision)

        if self._Lock is True:
            self.SetValue([Value, Value, Value])
        else:
            self.SetValueDim("Z", Value)

    def _OnSpinBoxXValueChanged(self, value):
        if self._Lock is True:
            self.SetValue([value, value, value])
        else:
            self.SetValueDim("X", value)

    def _OnSpinBoxYValueChanged(self, value):
        if self._Lock is True:
            self.SetValue([value, value, value])
        else:
            self.SetValueDim("Y", value)

    def _OnSpinBoxZValueChanged(self, value):
        if self._Lock is True:
            self.SetValue([value, value, value])
        else:
            self.SetValueDim("Z", value)

    def GetValue(self):
        return self._Value

    def SetValue(self, value):
        self._Value = list(value)
        self._UpdateUI()

        if self._Setter is not None:
            self._Setter(self._Value)

    def SetValueDim(self, dim, value):
        if dim is "X":
            self._Value[0] = value
        if dim is "Y":
            self._Value[1] = value
        if dim is "Z":
            self._Value[2] = value

        self._UpdateUI()

        if self._Setter is not None:
            self._Setter(self._Value)

    def _UpdateUI(self):
        self._SpinBoxes["X"].setValue(self._Value[0])
        self._SpinBoxes["Y"].setValue(self._Value[1])
        self._SpinBoxes["Z"].setValue(self._Value[2])

        self._Sliders["X"].setValue(self._Value[0] * self._Precision)
        self._Sliders["Y"].setValue(self._Value[1] * self._Precision)
        self._Sliders["Z"].setValue(self._Value[2] * self._Precision)

    def SetVisible(self, visible):
        if self._Label is not None:
            self._Label.setVisible(visible)

        for k, v in self._Sliders.items():
            v.setVisible(visible)

        for k, v in self._SpinBoxes.items():
            v.setVisible(visible)

    def SetXVisible(self, visible):
        self._Sliders["X"].setVisible(visible)
        self._SpinBoxes["X"].setVisible(visible)

    def SetYVisible(self, visible):
        self._Sliders["Y"].setVisible(visible)
        self._SpinBoxes["Y"].setVisible(visible)

    def SetZVisible(self, visible):
        self._Sliders["Z"].setVisible(visible)
        self._SpinBoxes["Z"].setVisible(visible)

    def SetEnabled(self, enabled):
        if self._Label is not None:
            self._Label.setEnabled(enabled)

        for k, v in self._Sliders.items():
            v.setEnabled(visible)

        for k, v in self._SpinBoxes.items():
            v.setEnabled(visible)

    def SetMinimum(self, minimum):
        for k, v in self._Sliders.items():
            v.setMinimum(minimum * self._Precision)
        for k, v in self._SpinBoxes.items():
            v.setMinimum(minimum)

    def SetMaximum(self, maximum):
        for k, v in self._Sliders.items():
            v.setMaximum(maximum * self._Precision)
        for k, v in self._SpinBoxes.items():
            v.setMaximum(maximum)

    def SetDecimals(self, decimals):
        for k, v in self._SpinBoxes.items():
            v.setDecimals(decimals)

    def SetPrefix(self, prefix):
        for k, v in self._SpinBoxes.items():
            v.setPrefix(prefix)

    def SetSuffix(self, suffix):
        for k, v in self._SpinBoxes.items():
            v.setSuffix(suffix)

    def SetLock(self, lock):
        if lock is 0:
            self._Lock = False
        else:
            self._Lock = True

    def _OnLock(self, lock):
        self.SetLock(lock)
