import sys

from pybind11 import get_cmake_dir
# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup

__version__ = "0.0.1"

# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)
ext_modules = [
    Pybind11Extension("gdelta",
        ["src/main.cpp"],
        define_macros = [('VERSION_INFO', __version__)],
        libraries=['gdelta'],
        cxx_std=17
        ),
]

setup(
    name="gdelta",
    version=__version__,
    author="Ferris Kwaijtaal",
    author_email="ferris+py@devdroplets.com",
    url="https://github.com/i404788/pygdelta",
    description="GDelta Bindings for Python",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    zip_safe=False,
    python_requires=">=3.6",
)
