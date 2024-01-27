FROM ubuntu:20.04

RUN mkdir /hack
WORKDIR /hack

RUN apt-get update && apt-get install -y wget

# need to match host kernel version: cat /proc/version
RUN wget https://www.kernel.org/pub/linux/kernel/v5.x/linux-5.4.tar.gz
RUN tar -xzf ./linux-5.4.tar.gz

# TODO: install kernel source tree in container from kernel.org and unzip
# https://unix.stackexchange.com/questions/115577/how-to-create-a-kernel-source-tree
# check if source tree installed on linux host at /usr/source/kernel/. . .

# install necessary resources to compiler kernel
# TODO: specify verions
RUN apt-get update && sudo apt-get -y install \ 
build-essential libncurses-dev bison flex libssl-dev libelf-dev
