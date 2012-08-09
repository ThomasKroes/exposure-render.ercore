
class SubjectMixin:
    def __init__(self):
        self.observers  = {}
        self.mutes      = {}

    def AddObserver(self, evt_type, o):
        if evt_type not in self.observers:
            self.observers[evt_type] = [o]

        elif o not in self.observers[evt_type]:
            self.observers[evt_type].append(o)

    def NotifyObservers(self, evt_type, evt):
        if evt_type in self.mutes or evt_type not in self.observers:
            return

        for o in self.observers[evt_type]:
            # invoke observers with ourselves as first param
            o(self, evt_type, evt)

    def Mute(self, evt_type):
        self.mutes[evt_type] = True

    def Unmute(self, evt_type):
        if evt_type in self.mutes:
            del self.mutes[evt_type]