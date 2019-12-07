name = "hyperlpr_python_pkg"
import sys
from .hyperlpr import LPR
import os

PR = LPR(os.path.join(os.path.split(os.path.realpath(__file__))[0],"models"))
def HyperLPR_plate_recognition(Input_BGR,minSize=30,charSelectionDeskew=True , region = "CH"):
    return PR.plate_recognition(Input_BGR,minSize,charSelectionDeskew)
