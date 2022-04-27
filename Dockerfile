FROM ubuntu:18.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
	build-essential \
	clang \
	cmake \
	git \
	lcov \
	libboost-dev \
	libgmp-dev \
	libmpfr-dev \
	ninja-build \
	qtdeclarative5-dev \
	sdcc=3.5.0+dfsg-2build1 \
	vim-tiny \
	wget

# install more up-to-date catch2
RUN wget http://mirrors.kernel.org/ubuntu/pool/universe/c/catch2/catch2_2.13.7-1_amd64.deb
RUN echo "3ca43a3b3347ec2e220e0cc6e9c38859  catch2_2.13.7-1_amd64.deb" | md5sum --check --
RUN dpkg -i catch2_2.13.7-1_amd64.deb
