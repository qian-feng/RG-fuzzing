# Use ubuntu 16.04
FROM ubuntu:xenial

# install packages needed 
RUN apt-get update && \
    apt-get install -y vim gdb build-essential \
    libc6-dev libc6-dev-i386 gcc-multilib g++-multilib \
    git wget cmake llvm-6.0-dev llvm-dev pkg-config \
    libjemalloc-dev clang clang-6.0 zlib1g-dev python3 \
    python python-pip python-dev \
    autoconf libtool nasm

RUN pip install xlsxwriter pycrypto 
#sysv_ipc

RUN ln -s /usr/bin/llvm-config-6.0 /usr/local/bin/llvm-config 
RUN ln -s /usr/bin/clang-6.0 /usr/local/bin/clang
RUN ln -s /usr/bin/clang-cpp-6.0 /usr/local/bin/clang++

# create root workdir
RUN mkdir -p cocofuzz

WORKDIR cocofuzz

VOLUME ["/data"]
WORKDIR /data
