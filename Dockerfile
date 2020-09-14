FROM ubuntu:18.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
	build-essential \
	catch \
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
	vim-tiny
