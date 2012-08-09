
from PyQt import *

from SubjectMixin import *
from AlignmentWidget import *

class ShapeWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('ShapeWidget.ui', self)

