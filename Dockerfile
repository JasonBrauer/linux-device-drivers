FROM ubuntu:20.04

# may not be needed if volume mount creates
RUN mkdir /lib/modules
RUN mkdir /hack

RUN mkdir /kernel
WORKDIR /kernel

# install necessary resources to compile kernel
# https://docs.kernel.org/admin-guide/quickly-build-trimmed-linux.html#:~:text=Install%20all%20software%20required%20to,libelf%27%20and%20%27openssl%27.
RUN apt-get update && \
apt-get -y install build-essential libncurses-dev bison flex libssl-dev libelf-dev bc

RUN apt-get install -y git
RUN git config --global --add safe.directory /hack

WORKDIR /hack