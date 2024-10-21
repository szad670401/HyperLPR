# !/usr/bin/env python
from setuptools import find_packages, setup
from hyperlpr3 import __version__ as lpr3_ver

__version__ = lpr3_ver

if __name__ == "__main__":
    setup(
        name="hyperlpr3",
        version=__version__,
        description="vehicle license plate recognition.",
        url="https://github.com/szad670401/HyperLPR",
        author="HyperInspire",
        author_email="tunmxy@163.com",
        keywords="vehicle license plate recognition",
        packages=find_packages(),
        classifiers=[
            "License :: OSI Approved :: Apache Software License",
            "Operating System :: OS Independent",
            "Programming Language :: Python :: 3",
        ],
        install_requires=[
            "opencv-python",
            "onnxruntime",
            "tqdm",
            "requests",
            "fastapi",
            "uvicorn",
            "python-multipart",
            "loguru"
        ],
        license="Apache License 2.0",
        zip_safe=False,
        entry_points="""
                [console_scripts]
                lpr3=hyperlpr3.command.cli:cli
            """
    )