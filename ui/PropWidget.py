
from PyQt import *

from SubjectMixin import *
from AlignmentWidget import *
from ShapeWidget import *
from EmitterWidget import *

class ObjectWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('PropWidget.ui', self)