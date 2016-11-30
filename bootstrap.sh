#!/usr/bin/env bash

# Create a symbolic link to the project.
if [ ! -L ThreadProfiler ]; then
    ln -s /vagrant ThreadProfiler
fi

# Install dependencies.
apt-get update
apt-get install build-essential cmake git g++ -y

# Create a temporary directory.
DEPS_DIR=$(mktemp -d)

# Change to the temporary directory.
pushd $DEPS_DIR
    if [ ! -f /usr/local/lib/libbenchmark.a ]; then
        echo "Could not find google-benchmark, installing it from sources."
        git clone https://github.com/google/benchmark.git
        pushd benchmark
            mkdir build
            pushd build
                cmake ..
                make
                sudo make install
            popd
        popd
    fi
popd

rm -rf $DEPS_DIR

pushd ThreadProfiler
    # Create a build directory if it does not exist.
    if [ ! -d build ]; then
        mkdir build
    fi

    pushd build
    cmake ..
    make
    popd
popd
