
from PyQt import *
from Edit.Edits import *
from Planning.KneePlanning import *

class Editable:
    def __init__(self):
        self._Edits = {}

        self._SetupEdits()

        Planning.AddObserver('planning:patientloaded', self._Load)
        QCoreApplication.instance().aboutToQuit.connect(self._Save)

    def _SetupEdits(self):
        print "_SetupEdits() not implemented!"
