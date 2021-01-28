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
RUN wget http://mirrors.kernel.org/ubuntu/pool/universe/c/catch2/catch2_2.13.0-1_all.deb
RUN echo "1d501c7f817cfcd46dd1b79edc10896d catch2_2.13.0-1_all.deb" | md5sum --check --
RUN dpkg -i catch2_2.13.0-1_all.deb
