
from PyQt import *

from SubjectMixin import *

class EmitterWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('EmitterWidget.ui', self)
