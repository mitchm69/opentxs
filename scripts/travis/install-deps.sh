#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Error: expected OS type argument." >&2
    exit 1
fi

os=$1

case "$os" in
    linux)
        sudo apt-get install libprotobuf-dev protobuf-compiler libzmq3-dev \
            libboost-all-dev doxygen
        ;;
    osx)
        brew update
        brew install protobuf-c protobuf boost openssl
        brew link --force openssl
        ;;
    *)
        echo "Error: unknown OS '$os'" >&2
        exit 1
        ;;
esac

git clone https://github.com/zeromq/cppzmq.git
sudo cp cppzmq/zmq.hpp /usr/local/include/
