import os
from .hyperlpr import LPR

PR = LPR(os.path.join(os.path.split(os.path.realpath(__file__))[0], "models"))


def HyperLPR_plate_recognition(Input_BGR, minSize=30, charSelectionDeskew=True, region="CH", DB=True):
    return PR.plate_recognition(Input_BGR, minSize, charSelectionDeskew, DB)
