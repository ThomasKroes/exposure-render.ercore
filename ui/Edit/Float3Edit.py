
from BaseEdit import *
from FloatEdit import *

class Float3Edit(BaseEdit):
    def __init__(self, parent, name, minimum, maximum, value = [0, 0, 0], precision = 1000, decimals = 3, prefix = "", suffix = ""):
        BaseEdit.__init__(self, name)

        self._Value = [0, 0, 0]

        self._FloatEdits = {}

        self._FloatEdits["X"] = FloatEdit(parent, name + "X", minimum, maximum, value[0], precision, decimals, prefix, suffix)
        self._FloatEdits["Y"] = FloatEdit(parent, name + "Y", minimum, maximum, value[1], precision, decimals, prefix, suffix)
        self._FloatEdits["Z"] = FloatEdit(parent, name + "Z", minimum, maximum, value[2], precision, decimals, prefix, suffix)

        self._AddControl("Label", parent.findChild(QLabel, name + "Label"))

    def SetVisible(self, visible):
        BaseEdit.SetVisible(self, visible)
        for k, fv in self._FloatEdits.items():
            fv.SetVisible(visible)

    def SetEnabled(self, enabled):
        BaseEdit.SetEnabled(self, enabled)
        for k, fv in self._FloatEdits.items():
            fv.SetEnabled(enabled)

    def SetMinimum(self, minimum):
        for k, fv in self._FloatEdits.items():
            fv.SetMinimum(minimum)

    def SetMaximum(self, maximum):
        for k, fv in self._FloatEdits.items():
            fv.SetMaximum(maximum)

    def SetDecimals(self, decimals):
        for k, fv in self._FloatEdits.items():
            fv.SetDecimals(decimals)

    def SetPrefix(self, prefix):
        for k, fv in self._FloatEdits.items():
            fv.SetPrefix(prefix)

    def SetSuffix(self, suffix):
        for k, fv in self._FloatEdits.items():
            fv.SetSuffix(suffix)

    def SetDelta(self, delta):
        for k, fv in self._FloatEdits.items():
            fv.SetDelta(delta)