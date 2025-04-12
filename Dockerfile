FROM ubuntu:24.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
	build-essential \
	catch2 \
	clang \
	cmake \
	git \
	lcov \
	libboost-dev \
	libgmp-dev \
	libmpfr-dev \
	ninja-build \
	qtdeclarative5-dev \
	vim-tiny \
	wget

# install older sdcc version which produces smaller binaries
RUN wget http://archive.ubuntu.com/ubuntu/pool/universe/s/sdcc/sdcc_3.5.0+dfsg-2build1_amd64.deb
RUN echo "2648593be8883e620a253bb96615d5c0  sdcc_3.5.0+dfsg-2build1_amd64.deb" | md5sum --check --
RUN wget http://archive.ubuntu.com/ubuntu/pool/universe/s/sdcc/sdcc-libraries_3.5.0+dfsg-2build1_all.deb
RUN echo "e5e0c4d851ced8d73c3f0de8daec579a  sdcc-libraries_3.5.0+dfsg-2build1_all.deb" | md5sum --check --
RUN dpkg -i sdcc_3.5.0+dfsg-2build1_amd64.deb sdcc-libraries_3.5.0+dfsg-2build1_all.deb

