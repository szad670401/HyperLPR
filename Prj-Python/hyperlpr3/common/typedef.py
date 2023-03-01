import numpy as np

PLATE_TYPE_BLUE = 0
PLATE_TYPE_GREEN = 1
PLATE_TYPE_YELLOW = 2

INFER_ONNX_RUNTIME = 0
INFER_MNN = 1

DETECT_LEVEL_LOW = 0
DETECT_LEVEL_HIGH = 1

MONO = 0    # 单层车牌
DOUBLE = 1  # 双层车牌

UNKNOWN = -1                         # 未知车牌
BLUE = 0                             # 蓝牌
YELLOW_SINGLE = 1                    # 黄牌单层
WHILE_SINGLE = 2                     # 白牌单层
GREEN = 3                            # 绿牌新能源
BLACK_HK_MACAO = 4                   # 黑牌港澳
HK_SINGLE = 5                        # 香港单层
HK_DOUBLE = 6                        # 香港双层
MACAO_SINGLE = 7                     # 澳门单层
MACAO_DOUBLE = 8                     # 澳门双层
YELLOW_DOUBLE = 9                    # 黄牌双层


def code_filter(code: str) -> int:
    plate_type = UNKNOWN
    if code[0] == 'W' and code[1] == 'J':
        plate_type = WHILE_SINGLE
    elif len(code) == 8:
        plate_type = GREEN
    elif '学' in code:
        plate_type = BLUE
    elif '港' in code:
        plate_type = BLACK_HK_MACAO
    elif '澳' in code:
        plate_type = BLACK_HK_MACAO
    elif '警' in code:
        plate_type = WHILE_SINGLE
    elif '粤Z' in code:
        plate_type = BLACK_HK_MACAO

    return plate_type


class Plate(object):

    def __init__(self,
                 vertex: np.ndarray,
                 plate_code: str,
                 rec_confidence: float,
                 det_bound_box,
                 dex_bound_confidence: float,
                 plate_type: int):
        assert vertex.shape == (4, 2)
        self.vertex = vertex
        self.det_bound_box = det_bound_box
        self.plate_code = plate_code
        self.rec_confidence = rec_confidence
        self.dex_bound_confidence = dex_bound_confidence

        self.left_top, self.right_top, self.right_bottom, self.left_bottom = vertex
        self.plate_type = plate_type

    def to_dict(self):
        return dict(plate_code=self.plate_code, rec_confidence=self.rec_confidence,
                    det_bound_box=self.det_bound_box, plate_type=self.plate_type)

    def to_result(self):
        return [self.plate_code, self.rec_confidence, self.plate_type, self.det_bound_box.tolist(),]

    def to_full_result(self):
        return [self.plate_code, self.rec_confidence, self.plate_type, self.det_bound_box.tolist(), self.vertex.tolist()]

    def __dict__(self):
        return self.to_dict()

    def __str__(self):
        return str(self.to_dict())
