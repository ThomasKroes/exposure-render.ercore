
from PyQt import *

def LoadBool(name, default):
    Settings = QSettings("TU Delft", "OrthoAssist")
    return Settings.value(name, default).toBool()

def LoadInt(name, default):
    Settings = QSettings("TU Delft", "OrthoAssist")
    return Settings.value(name, default).toInt()[0]

def LoadFloat(name, default):
    Settings = QSettings("TU Delft", "OrthoAssist")
    return Settings.value(name, default).toFloat()[0]

def Save(name, value):
    Settings = QSettings("TU Delft", "OrthoAssist")
    Settings.setValue(name, value)
