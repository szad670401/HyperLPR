from abc import ABCMeta, abstractmethod

class HamburgerABC(metaclass=ABCMeta):
    def __init__(self, input_size: tuple = None, *args, **kwargs):
        """
        Initialize the HamburgerABC class.

        :param input_size: Tuple indicating the input size (e.g., (width, height)).
        :param args: Additional positional arguments.
        :param kwargs: Additional keyword arguments.
        """
        self.input_size = input_size
        self.running = False

    @abstractmethod
    def _run_session(self, data) -> any:
        """
        Abstract method to run the main session.

        :param data: Preprocessed data.
        :return: The result of the session.
        """
        pass

    @abstractmethod
    def _postprocess(self, data) -> any:
        """
        Abstract method to postprocess the data.

        :param data: Data from the session.
        :return: Postprocessed result.
        """
        pass

    @abstractmethod
    def _preprocess(self, image) -> any:
        """
        Abstract method to preprocess the image.

        :param image: Input image.
        :return: Preprocessed data.
        """
        pass

    def __call__(self, image, *args, **kwargs):
        """
        Process the image through the whole pipeline: preprocess, run session, and postprocess.

        :param image: Input image.
        :return: Final result after postprocessing.
        """
        flow = self._preprocess(image)
        flow = self._run_session(flow)
        result = self._postprocess(flow)
        return result

    def is_running(self) -> bool:
        return self.running

