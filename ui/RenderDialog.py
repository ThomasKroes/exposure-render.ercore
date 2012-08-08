
from PyQt import *

class RenderDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)

        uic.loadUi('RenderDialog.ui', self)


