
from PyQt import *

from SubjectMixin import *
from AlignmentWidget import *
from ShapeWidget import *
from EmitterWidget import *

class ObjectWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('ObjectWidget.ui', self)

        self._AlignmentWidget   = AlignmentWidget()
        self._ShapeWidget       = ShapeWidget()
        self._EmitterWidget     = EmitterWidget()

        self.layout().setContentsMargins(0, 0, 0, 0)

        self.layout().addWidget(self._AlignmentWidget)
        self.layout().addWidget(self._ShapeWidget)
        self.layout().addWidget(self._EmitterWidget)