
import os, sys

from Application import *
from MainWindow import *
from RenderDialog import *

def Main():

    App = Application(sys.argv)
    App.setApplicationName('Exposure Render')

    App.setStyle(QStyleFactory.create("Cleanlooks"))

    MW = MainWindow()
    MW.show()

    #RD = RenderDialog()
    #RD.show()
    sys.exit(App.exec_())

if __name__ == '__main__':
    Main()