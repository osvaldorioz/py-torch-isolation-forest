from setuptools import setup
from torch.utils.cpp_extension import CppExtension, BuildExtension
import pybind11

setup(
    name="isolation_forest",
    ext_modules=[
        CppExtension(
            "isolation_forest",
            ["isolation_forest.cpp"],
            include_dirs=[pybind11.get_include()],
            extra_compile_args=["-std=c++17", "-O3"],
        )
    ],
    cmdclass={"build_ext": BuildExtension},
)
