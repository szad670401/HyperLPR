import numpy as np
import MNN
from loguru import logger


class MNNAdapter(object):

    def __init__(self, model_path: str, input_shape: tuple,
                 dim_type: int = MNN.Tensor_DimensionType_Caffe, outputs_name=None, outputs_shape=None):
        self.interpreter = MNN.Interpreter(model_path)
        self.session = self.interpreter.createSession()
        self.input_tensor = self.interpreter.getSessionInput(self.session)
        self.input_shape = input_shape
        self.dim_type = dim_type
        self.outputs_name = outputs_name
        self.outputs_shape = outputs_shape

    def inference(self, tensor: np.ndarray) -> np.ndarray:
        tensor = tensor.astype(np.float32)
        tmp_input = MNN.Tensor(self.input_shape, MNN.Halide_Type_Float, tensor, self.dim_type)
        self.input_tensor.copyFrom(tmp_input)
        self.interpreter.runSession(self.session)
        output_tensor = list()
        if self.outputs_name:
            if self.outputs_shape:
                for idx, shape in enumerate(self.outputs_shape):
                    tmp_output = MNN.Tensor(shape, MNN.Halide_Type_Float, np.ones(shape).astype(np.float32), self.dim_type)
                    tmp_tensor = self.interpreter.getSessionOutput(self.session, self.outputs_name[idx])
                    tmp_tensor.copyToHostTensor(tmp_output)
                    output_tensor.append(np.asarray(tmp_output.getData()))
            else:
                output_tensor = [np.asarray(self.interpreter.getSessionOutput(self.session, name).getData()) for name in self.outputs_name]
        else:
            output_tensor.append(self.interpreter.getSessionOutput(self.session).getData())
        res = np.asarray(output_tensor)

        return res
