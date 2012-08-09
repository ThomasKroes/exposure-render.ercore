
from PyQt import *

from SubjectMixin import *
from LightsModel import *

class LightsWidget(QWidget, SubjectMixin):
    def __init__(self):
        QWidget.__init__(self)
        SubjectMixin.__init__(self)

        uic.loadUi('LightsWidget.ui', self)

        self.AddLightPushButton.clicked.connect(self._OnAddLight)
        self.RemoveLightPushButton.clicked.connect(self._OnRemoveLight)

    def _OnAddLight(self):
        self.NotifyObservers("light:add", None)

    def _OnRemoveLight(self):
        self.NotifyObservers("light:remove", None)