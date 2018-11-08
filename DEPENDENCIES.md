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

# Build dependencies

*Instructions for MetroCollect 2.3.4*


## Snap plugin C++ library

This library is available on [GitHub](https://github.com/Maxime999/snap-plugin-lib-cpp), and it is provided with MetroCollect as a git submodule in the `third_party/` directory.


## Snap library dependencies

The Snap plugin C++ library has various dependencies:

| Name          | Website                             | Git                                                 | Download                                                                                         |
|---------------|-------------------------------------|-----------------------------------------------------|--------------------------------------------------------------------------------------------------|
| Boost         | [Official](https://www.boost.org/)  | [GitHub](https://github.com/boostorg/boost/)        | [Version 1.68.0, bz2](https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz) |
| gRPC          | [Official](https://grpc.io/)        | [GitHub](https://github.com/grpc/grpc/)             | [Version 1.15.1, gz](https://github.com/grpc/grpc/archive/v1.15.1.tar.gz)                        |
| cpp-netlib    | [Official](https://cpp-netlib.org/) | [GitHub](https://github.com/cpp-netlib/cpp-netlib/) | [Version 0.13.0, bz2](http://downloads.cpp-netlib.org/0.13.0/cpp-netlib-0.13.0-rc1.tar.bz2)      |
| spdlog        |                                     | [GitHub](https://github.com/gabime/spdlog/)         | [Version 1.2.0, gz](https://github.com/gabime/spdlog/archive/v1.2.0.tar.gz)                      |
| nlohmann json |                                     | [GitHub](https://github.com/nlohmann/json/)         | [Version 3.3.0](https://github.com/nlohmann/json/releases/download/v3.3.0/json.hpp)              |



The nlohmann json header file may be zipped in in a bz2 archive: `tar -cf - ./json.hpp | bzip2 > nlohmann-json-3.3.0.tar.bz2`

All these libraries should be unzipped to MetroCollect's `third_party/` directory.


## gRPC dependencies

In addition to Snap dependencies, gRPC also has three mandatory dependencies (among other optional dependencies). They can be downloaded automatically with git (`git submodule update --init`) or with the links below:

| Name      | Website                                                   | Git                                            | Download                                                                                                          |
|-----------|-----------------------------------------------------------|------------------------------------------------|-------------------------------------------------------------------------------------------------------------------|
| Protobuf  | [Official](https://github.com/protocolbuffers/protobuf/)  | [GitHub](https://github.com/boostorg/boost/)   | [Version 3.6.1, gz](https://github.com/protocolbuffers/protobuf/archive/v3.6.1.tar.gz)                            |
| BoringSSL | [Official](https://boringssl.googlesource.com/boringssl/) | [GitHub](https://github.com/google/boringssl/) | [Commit b29b21a, gz](https://github.com/google/boringssl/archive/b29b21a81b32ec273f118f589f46d56ad3332420.tar.gz) |
| c-ares    | [Official](https://c-ares.haxx.se/)                       | [GitHub](https://github.com/c-ares/c-ares/)    | [Version 1.13.0, gz](https://github.com/c-ares/c-ares/archive/cares-1_13_0.tar.gz)                                |

Protobuf and BoringSSL should be unzipped to MetroCollect's `third_party/grpc/third_party/` directory, and c-ares in `third_party/grpc/third_party/cares/`.


## Buidling
When everything is downloaded and unzipped in the right directories, you can go to MetroCollect's root folder and execute `./buildall.sh`. This will build all dependencies in a local directory, and after some time MetroCollect will be available in the `output/` directory.
