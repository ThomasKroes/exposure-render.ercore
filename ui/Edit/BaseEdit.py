
from PyQt import *
from Settings import *

class BaseEdit:
    def __init__(self, name, setter = None):
        self._Name      = name
        self._Setter    = setter
        self._Value     = None
        self._Verbose   = LoadBool("general/verbose", True)

    def GetName(self):
        return self._Name

    def SetSetter(self, setter):
        self._Setter = setter

    def GetSetter(self):
        return self._Setter

    def Connect(self, setter, value):
        self.SetSetter(setter)
        self.SetValue(value)
