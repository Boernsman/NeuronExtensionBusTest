# Neuron Extension Bus Test Application

[![CI](https://github.com/Boernsman/NeuronExtensionBusTest/actions/workflows/ci.yml/badge.svg)](https://github.com/Boernsman/NeuronExtensionBusTest/actions/workflows/ci.yml)

This project is a command-line utility written in C++20 to test Modbus communication over a serial connection. It allows the user to select serial ports, configure baud rates, set parity, specify Modbus slave addresses, and perform multiple register reads.

## Features

* Lists available serial ports.
* Allows user to select serial ports interactively or via command line arguments.
* Supports configurable baud rate, parity, and Modbus slave address.
* Uses libmodbus to communicate with Modbus devices.
* Measures the elapsed time and error count over a configurable number of requests.
* Provides a command-line interface for ease of use.

## Requirements

* Linux (Ubuntu 22.04 or later). The port scan reads `/dev`, so the tool is Linux-only.
* CMake 3.20 or higher.
* A C++20 compatible compiler (GCC 11 or newer).
* pkg-config.
* libmodbus.
* Boost.ProgramOptions.
* GoogleTest (only needed to build the unit tests).

On Debian/Ubuntu:

```
sudo apt-get install cmake g++ pkg-config libmodbus-dev \
    libboost-program-options-dev libgtest-dev
```

## Building the project

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The binaries are written to `build/`:

* `build/neuron_extension_bus_test` - the command line utility.
* `build/neuron_extension_bus_test_tests` - the unit tests.

### Running the tests

```
ctest --test-dir build --output-on-failure
```

### Building with Docker

If you do not want to install the dependencies on your host, the provided
`Dockerfile` builds the project (and runs the tests) inside a container:

```
docker build -t neuron-extension-bus-test .
```

## Running the Application

You can run the application using the following command:

```
./build/neuron_extension_bus_test [options]
```

Your user needs read/write access to the serial port. On most distributions
this means being a member of the `dialout` group:

```
sudo usermod -aG dialout $USER
```

### Command Line Options

-s, --serial <port>: Set the serial port.

-b, --baud <baudrate>: Set the baud rate (default: 19200).

-p, --parity <parity>: Set parity (even, none, or odd, default: none).

-a, --address <addr>: Set the slave address (default: 1).

-c, --count <count>: Set the request count (default: 1000).

-h, --help: Show help message.

If no serial port is provided, the program will prompt the user to select from the available ports.

Each request reads 5 holding registers starting at address 0. After all requests have been
sent, the total elapsed time and the number of failed requests are printed.

## Example Usage

```
./build/neuron_extension_bus_test -s /dev/ttyUSB0 -b 115200 -p even -a 10 -c 500
```

## License

MIT License

Copyright (c) 2024 Neuron Extension Bus Test Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

