FROM ubuntu:22.04

# Install base packages
RUN set -ex && \
    apt-get update && \
    apt-get install -y lsb-release wget software-properties-common zlib1g-dev libpng++-dev libjpeg-dev libomp-dev

# Install clang toolchain
RUN wget https://apt.llvm.org/llvm.sh &&\
    chmod +x llvm.sh && \
    ./llvm.sh 15

# Install git, cmake, gdb
RUN set -ex && \
    apt-get update && \
    apt-get install -y git cmake gdb

RUN ln -s /usr/bin/clang++-15 /usr/bin/clang++

RUN mkdir /scratch && \
	cd /scratch && \
	wget https://github.com/halide/Halide/releases/download/v16.0.0/Halide-16.0.0-x86-64-linux-1e963ff817ef0968cc25d811a25a7350c8953ee6.tar.gz && \
    tar zxvf Halide-16.0.0-x86-64-linux-1e963ff817ef0968cc25d811a25a7350c8953ee6.tar.gz
