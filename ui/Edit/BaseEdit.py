
from PyQt import *
from Settings import *

class BaseEdit:
    def __init__(self, name, setter = None):
        self._Name      = name
        self._Setter    = setter
        self._Value     = None
        self._Verbose   = LoadBool("general/verbose", True)
        self._Controls  = {}

    def _AddControl(self, id, control):
        if control is not None:
            self._Controls[id] = control

    def GetName(self):
        return self._Name

    def SetSetter(self, setter):
        self._Setter = setter

    def GetSetter(self):
        return self._Setter

    def Connect(self, setter, value):
        self.SetSetter(setter)
        self.SetValue(value)

    def SetVisible(self, visible):
        for key, c in self._Controls.items():
            c.setVisible(visible)

    def SetEnabled(self, enabled):
        for key, c in self._Controls.items():
            c.setEnabled(enabled)
