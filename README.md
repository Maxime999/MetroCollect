# MetroCollect

[![Build Status](https://travis-ci.org/Maxime999/MetroCollect.svg?branch=master)](https://travis-ci.org/Maxime999/MetroCollect)

<!--
Copyright 2018 CFM (www.cfm.fr)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->


This project aims to collect and process various system metrics (such as CPU, memory and network usage) very efficiently, so that they can be retrieved multiple times per second (for instance, every 50ms). It is designed to work with [Snap Telemetry](http://snap-telemetry.io).

This repo is organized as the following:
 * Two C++ libraries, `CircularArray` and `MetroCollect`
 * Three standalone interfaces using the aforementioned libraries: `MetroCollectValues` (which collects and prints values), `MetroCollectStats` (which collects, compute stats and print them) and `MetroCollectFile` (which collects and write values into CSV format)
 * The Snap plugin interface, `MetroCollectSnap`, which can collect values, compute stats and send either of them to the Snap daemon. It works with a fork of the [C++ Snap plugin library](https://github.com/Maxime999/snap-plugin-lib-cpp)

Refer to [USAGE.md](USAGE.md) for more information on how to use these programs.

MetroCollect's documentation is available on [GitHub pages](https://maxime999.github.io/MetroCollect/).


## Readme Contents
- [MetroCollect](#metrocollect)
    - [Readme Contents](#readme-contents)
    - [Getting started](#getting-started)
        - [Operating systems](#operating-systems)
        - [System Requirements](#system-requirements)
        - [Compiling](#compiling)
        - [Compiling dependencies](#compiling-dependencies)
        - [Configuration and Usage](#configuration-and-usage)
    - [Documentation](#documentation)
    - [Community Support](#community-support)
    - [License](#license)
    - [Acknowledgements](#acknowledgements)


## Getting started
### Operating systems
All OSs currently supported by plugin:
* Linux/amd64

### System Requirements
A C++ 17 compiler such as GCC 7 is required.

The libraries and standalone interfaces do not have additional dependencies. The Snap plugin interface requires the [C++ Snap plugin library](https://github.com/Maxime999/snap-plugin-lib-cpp) and its dependencies, which should be installed on the system first. MetroCollect uses static linking of those libraries for the programs to be portable.

Refer to the DEPENDENCIES.md file for more information.


### Compiling
If the Snap and its dependencies are built in the `/usr/local/lib`, execute `ldconfig /usr/local/lib/` to allow the `ld` linker to look for installed libraries there.

Then you can compile with CMake:

``` bash
cd MetroCollect
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

Other build configured types are `Debug`, `Release`, and `RelWithDebInfo` (to profile with [Linux Perf](https://perf.wiki.kernel.org/index.php/Main_Page) or [GPerfTools](https://gperftools.github.io/gperftools/)).

### Compiling dependencies
The `buildall.sh` script can be used to build MetroCollect Snap Plugin library and its dependencies automatically. It can either download the dependencies from GitHub or use local ones in the `third_party` folder.

### Configuration and Usage
Refer to [USAGE.md](USAGE.md)


## Documentation
The documentation can be built with Doxygen from the `build` directory. It is installed in `doc/html`.

``` bash
cd MetroCollect
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make doc
make install
```


## Community Support
This repository is one of **many** plugins in **Snap**, a powerful telemetry framework. The full project is at http://github.com/intelsdi-x/snap.
To reach out on other use cases, visit [Slack](http://slack.snap-telemetry.io).


## License
[Snap](http://github.com/intelsdi-x/snap), along with this plugin, is an Open Source software released under the Apache 2.0 [License](LICENSE).


## Acknowledgements
* Author: [Maxime Amossé](https://github.com/Maxime999), on behalf of [Capital Fund Management](https://www.cfm.fr)
