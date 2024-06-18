# Wazuh-Agent POC

This repository is a proof of concept for the Wazuh Agent. Follow the instructions below to set up and build the project.

## Cloning the Repository

First, initialize the repository's submodule:

```sh
git submodule update --init --recursive
```

## Cloning and Setting Up vcpkg

Next, clone the vcpkg repository inside the `./poc/` subdirectory:

```sh
git clone https://github.com/microsoft/vcpkg.git ./poc/vcpkg
```

Navigate to the vcpkg directory and run the bootstrap script:

```sh
cd ./poc/vcpkg
./bootstrap-vcpkg.sh
```

## Building the Project

Navigate to the curl directory and build the project:

```sh
cd ./poc/curl
mkdir build
cd build
cmake ..
make
```
