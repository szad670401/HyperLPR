from abc import ABCMeta, abstractmethod


class HamburgerABC(metaclass=ABCMeta):

    def __init__(self, input_size: tuple = None, *args,
                 **kwargs):
        self.input_size = input_size

    @abstractmethod
    def _run_session(self, data):
        pass

    @abstractmethod
    def _postprocess(self, data):
        pass

    @abstractmethod
    def _preprocess(self, image):
        pass

    def __call__(self, image):
        flow = self._preprocess(image)
        flow = self._run_session(flow)
        result = self._postprocess(flow)

        return result
