
from PyQt import *
from Edit.Edits import *
from SubjectMixin import *

class Editable(SubjectMixin):
    def __init__(self):
        SubjectMixin.__init__(self)

        self._Edits = {}

        self._SetupEdits()

        QCoreApplication.instance().aboutToQuit.connect(self._Save)

    def _SetupEdits(self):
        print "_SetupEdits() not implemented!"


