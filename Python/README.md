# Sysboard DAC ADC

This module can be used to control the Sysboard ADCs and DAC using python.

# Installation
The easiest way to install is to clone the repository and use pip to do an editable install. For example from within a virutal environment with pip installed, the following commands would install the package:

``` bash
$ git clone git@github.com:illusense/illu-sysb-dacadc.git
$ cd illu-sysb-dacadc
$ pip install -e .
```
Now the package is available for import, and any changes made to the local source files will be reflected when the code is called.

## Generating the protobuf extensions

The Sysbdacadc class makes use of a protocol buffers definition defined in `sysb-dacadc.proto`. If this definition is changed, the corresponding python and/or C code can be regenerated using the included `setup.py` build commands:

``` bash
$ python setup.py proto_build --build-all
```
This will generate both the python and C source files. The python files are generated using google's protoc, and the C source files are generated using the nanopb_generator protoc extension. Nanopb generated statically allocated C suitable to be run on a Microcontroller. By default, the script will attempt to deposit the python source file into the `sysb-dacadc_py` directory (as this is where the Sysbdacadc module expects to find it), and the C source files into the `sysb-dacadc_c` directory. If these directories do not exist, the script will fail. The script can be instructed to deposit the generated source files elsewhere, if desired:

``` bash
$ python setup.py proto_build --build-python --py-output-dir=<location for the source files>
$ python setup.py proto_build --build-c --c-output-dir=<location for the source files>
```

If the `Sysbdacadc` package was installed with the procedure above, then the `nanopb` and `protoc` dependencies should already be present in your system/virtual environment. Alternatively, protoc can be located using the `PROTOC` environment variable and nanopb_generator with the `NANOPB`:

``` bash
$ export PROTOC=<path to protoc>
$ export NANOPB=<path to nanopb_generator>
```

Another alternative is to directly specify the paths when calling the `setup.py` build script:

``` bash
$ python setup.py proto_build --protoc=<some other protoc binary>
$ python setup.py proto_build --nanopb=<some other nanopb_generator binary>
```
