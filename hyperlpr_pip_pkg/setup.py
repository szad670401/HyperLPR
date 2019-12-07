import setuptools

# with open("README.md", "r") as fh:
#     long_description = fh.read()
long_description = "LPR framework"
setuptools.setup(
    name="hyperlpr",
    version="0.0.2",
    author="Jack Yu",
    author_email="jack-yu-business@foxmail.com",
    description="A general small license plate recognition framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/zeusees/HyperLPR",
    packages=setuptools.find_packages(),
    classifiers=(
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 2",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ),
    install_requires=[
        "opencv-python"
    ],
    package_data={
        "hyperlpr": [
            "models/cascade/char/*.*",
            "models/cascade/detector/*.*",
            "models/dnn/*.*"
        ]
    },

)
