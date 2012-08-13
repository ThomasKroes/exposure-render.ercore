
from PyQt import *
from ExposureRender import *

from SubjectMixin import *

class EmitterWidget(QWidget, SubjectMixin):
    def __init__(self, prop):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        self._Prop = prop

        uic.loadUi("Designer/EmitterWidget.ui", self)
