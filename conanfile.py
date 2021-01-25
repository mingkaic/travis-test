import os
import subprocess

from conans import ConanFile, CMake

def get_version():
    get_vers = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'get_version.sh')
    version = subprocess.check_output(['bash', get_vers]).decode('utf-8')
    return version.strip()

class CppkgConan(ConanFile):
    name = "cppkg"
    version = get_version()
    license = "MIT"
    author = "Ming Kai Chen <mingkaichen2009@gmail.com>"
    url = "https://github.com/mingkaic/cppkg"
    description = "C++ utility packages."
    topics = ["conan", "utility"]
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "boost/1.73.0",
        "grpc/1.29.1@inexorgame/stable",
        "gtest/1.10.0",
    )
    generators = "cmake", "cmake_find_package_multi"

    options = {
        "fPIC": [True, False],
    }
    default_options = {
        "fPIC": True,
    }

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = self.options.fPIC
        cmake.configure()
        return cmake

    def configure(self):
        if self.settings.os == "Windows" and self.settings.compiler == "Visual Studio":
            del self.options.fPIC
            compiler_version = tools.Version(self.settings.compiler.version)
            if compiler_version < 14:
                raise ConanInvalidConfiguration("gRPC can only be built with Visual Studio 2015 or higher.")

    def source(self):
        self.run("git clone {}.git .".format(self.url))
        self.run("git checkout developer-fmts")

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE.*", dst="licenses", keep_path=False)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = self.name
        self.cpp_info.names["cmake_find_package_multi"] = self.name
        self.cpp_info.libs = ["diff", "egrpc", "error", "estd", "flag", "fmts", "logs"]
