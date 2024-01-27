FROM ubuntu:jammy

RUN mkdir /hack

# may not need this, can look up gcc version needed with /proc/version
# check gcc versions available with apt-cache. . 
RUN apt update && apt install linux-doc

# TODO: install kernel source tree in container from kernel.org and unzip
# https://unix.stackexchange.com/questions/115577/how-to-create-a-kernel-source-tree
# check if source tree installed on linux host at /usr/source/kernel/. . .

# install necessary resources to compiler kernel
# TODO: specify verions
RUN apt-get update && sudo apt-get -y install \ 
build-essential libncurses-dev bison flex libssl-dev libelf-dev
