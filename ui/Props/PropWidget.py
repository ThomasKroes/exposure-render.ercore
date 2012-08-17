
from PyQt import *

from SubjectMixin import *

class PropWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi("Designer/PropWidget.ui", self)