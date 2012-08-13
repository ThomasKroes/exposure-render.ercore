
from PyQt import *

class BaseEdit:
    def __init__(self, setter = None):
        self._Setter    = setter
        self._Value     = None

    def SetSetter(self, setter):
        self._Setter = setter

    def GetSetter(self):
        return self._Setter

    def Connect(self, setter, value):
        self.SetSetter(setter)
        self.SetValue(value)
