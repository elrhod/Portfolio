from setuptools import setup
import os
import sys
from distutils.spawn import find_executable
import subprocess
from setuptools import Command

class GenerateProto(Command):
    """ Setuptools command for generate both python and C code from any proto
    files found in the current directory.
    """
    description = 'Generate protocol buffer code from .proto files.'

    user_options = [
        ('protoc=', 'b', 'path to the protoc binary'),
        ('nanopb=', 'n', 'path to the nanopb_generator binary'),
        ('py-output-dir=', 'p', 'directory to deposit generated python code'),
        ('c-output-dir=', 'c', 'directory to deposit generated c code'),
        ('build-python', 'P', 'build the python extension'),
        ('build-c', 'C', 'build the C library'),
        ('build-all', 'A', 'build all libraries/extensions')
    ]

    def initialize_options(self):
        self.proto_src = None
        self.protoc = None
        self.nanopb = None
        self.py_output_dir = None
        self.c_output_dir = None
        self.build_python = None
        self.build_c = None
        self.build_all = None

    def finalize_options(self):
        if self.proto_src is None:
            raise Exception("Parameter --proto-src is missing")
        if self.py_output_dir is None:
            raise Exception("Parameter --py-output-dir is missing")
        if self.c_output_dir is None:
            raise Exception("Parameter --c-output-dir is missing")

        if self.build_all is not None:
            self.build_python = True
            self.build_c = True
        elif self.build_python == 1 and self.build_c == 1:
            self.build_python = True
            self.build_c = True
        elif self.build_python == 1 and self.build_c is None:
            self.build_python = True
            self.build_c = False
        elif self.build_python is None and self.build_c == 1:
            self.build_python = False
            self.build_c = True
        else:
            raise Exception("Must specify either --build-all or one of --build-python or --build-c")

    def find_protoc(self):
        """Find the Protocol Compiler.
        If you installed nanopb with pip or conda, this should be in the local
        install bin of illuscanner. Otherwise it can be specified system wide
        with the PROTOC environment vairable.
        """
        if 'PROTOC' in os.environ and os.path.exists(os.environ['PROTOC']):
            protoc = os.environ['PROTOC']
        else:
            protoc = find_executable("protoc")

        self.protoc = protoc

    def find_nanopb(self):
        """Find the Nanopb protoc extension binary.
        If you installed nanopb with pip or conda, this should be in the local
        install bin of illuscanner. Otherwise it can be specified system wide
        with the NANOPB environment vairable.
        """
        if 'NANOPB' in os.environ and os.path.exists(os.environ['NANOPB']):
            nanopb = os.environ['NANOPB']
        else:
            nanopb = find_executable("nanopb_generator")

        self.nanopb = nanopb

    def _generate_proto_py(self, source_file):
        """Invokes the Protocol Compiler to generate a _pb2.py from the given
        .proto file
        """
        dest_file = source_file.replace(".proto", "_pb2.py")

        if not os.path.exists(source_file):
            sys.stderr.write(f"Can't find required file: {source_file}\n")
            sys.exit(-1)

        if self.protoc == None:
            sys.stderr.write("Protocol buffers compiler 'protoc' not installed or not found.\n")
            sys.exit(-1)

        protoc_command = [self.protoc, "-I.", "--python_out="+self.py_output_dir, source_file]

        if self.verbose:
            print(f"Generating Python for `{source_file}`:")
            print(f"\t{' '.join(protoc_command)}")
        if subprocess.call(protoc_command) != 0:
            sys.exit(-1)

    def _generate_proto_c(self, source_file):
        """Invokes the nanopb compliler to generate .c and .h files from the given
        .proto file.
        """
        dest_file = source_file.replace(".proto", "_pb2.c")
        dest_file_header = source_file.replace(".proto", "_pb2.h")

        if not os.path.exists(source_file):
            sys.stderr.write(f"Can't find required file: {source_file}\n")
            sys.exit(-1)

        if self.nanopb == None:
            sys.stderr.write("Nanopb generator 'nanopb_generator' not installed or not found.\n")
            sys.exit(-1)

        nanopb_command = [self.nanopb, "-I.", "-D"+self.c_output_dir, source_file]

        if self.verbose:
            print(f"Generating C for `{source_file}`:")
            print(f"\t{' '.join(nanopb_command)}")
        if subprocess.call(nanopb_command) != 0:
            sys.exit(-1)

    def run(self):
        for file_name in self.proto_src:
            if self.build_python:
                if self.protoc is None:
                    self.find_protoc()

                self._generate_proto_py(file_name)

            if self.build_c:
                if self.nanopb is None:
                    self.find_nanopb()

                self._generate_proto_c(file_name)

setup(
    name='sysb_dacadc',
    version='0.1',
    url='',
    author='Eduardo Luis Rhod',
    author_email='eduardo@illusense.com',
    description='Routines for using the sysboard adc and dac generically',
    packages=['sysb_dacadc'],
    install_requires=['numpy', 'pyserial', 'crcmod', 'protobuf', 'nanopb', 'protoc-wheel-0'],
    cmdclass={'proto_build': GenerateProto},
    options={'proto_build':{
        'proto_src': ['sysb_dacadc.proto'],
        'py_output_dir': 'sysb_dacadc',
        'c_output_dir': 'sysb_dacadc_c',
    }}
)
