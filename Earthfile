VERSION 0.8

build:
    FROM busybox

    WORKDIR /workspace
    RUN mkdir build
    COPY +build-amd64/build/* build/
    COPY +build-arm64/build/* build/
    COPY +build-armhf/build/* build/

    SAVE ARTIFACT build

build-base:
    FROM debian:buster

    COPY earthly/files/etc/apt/sources.list /etc/apt/sources.list

    RUN apt-get update && \
        apt-get install -y \
            build-essential \
            cmake \
            git

    RUN echo 'deb [trusted=yes] https://repo.goreleaser.com/apt/ /' | tee /etc/apt/sources.list.d/goreleaser.list
    RUN apt-get update && \
        apt-get install -y nfpm

    ENV TICKTOCK_VERSION 1.1.0

    WORKDIR /workspace/ticktock
    COPY . .

build-amd64:
    FROM +build-base
    
    ENV TARGET_ARCH=amd64

    RUN dpkg --add-architecture ${TARGET_ARCH}
    RUN apt-get update && \
        apt-get install -y \
            gcc-x86-64-linux-gnu \
            g++-x86-64-linux-gnu \
            zlib1g-dev:${TARGET_ARCH} \
            libmosquitto-dev:${TARGET_ARCH}

    RUN BRANCH=$(git rev-parse --abbrev-ref HEAD) \
        COMMIT=$(git rev-parse --short HEAD) \
        CTIME=$(date +%s) \
        cmake -DCMAKE_TOOLCHAIN_FILE=$(pwd)/cmake-toolchains/x86_64.cmake -S .
    RUN make # VERBOSE=1
    SAVE ARTIFACT bin

    WORKDIR nfpm
    RUN mkdir -p build
    RUN nfpm package -p deb -t build/ticktock_${TICKTOCK_VERSION}_${TARGET_ARCH}.deb
    RUN nfpm package -p rpm -t build/ticktock_${TICKTOCK_VERSION}_${TARGET_ARCH}.rpm
    SAVE ARTIFACT build

build-arm64:
    FROM +build-base

    ENV TARGET_ARCH=arm64

    RUN dpkg --add-architecture ${TARGET_ARCH}
    RUN apt-get update && \
        apt-get install -y \
            gcc-aarch64-linux-gnu \
            g++-aarch64-linux-gnu \
            zlib1g-dev:${TARGET_ARCH} \
            libmosquitto-dev:${TARGET_ARCH}

    RUN BRANCH=$(git rev-parse --abbrev-ref HEAD) \
        COMMIT=$(git rev-parse --short HEAD) \
        CTIME=$(date +%s) \
        cmake -DCMAKE_TOOLCHAIN_FILE=$(pwd)/cmake-toolchains/aarch64.cmake -S .
    RUN make # VERBOSE=1
    SAVE ARTIFACT bin

    WORKDIR nfpm
    RUN mkdir -p build
    RUN nfpm package -p deb -t build/ticktock_${TICKTOCK_VERSION}_${TARGET_ARCH}.deb
    RUN nfpm package -p rpm -t build/ticktock_${TICKTOCK_VERSION}_${TARGET_ARCH}.rpm
    SAVE ARTIFACT build

build-armhf:
    FROM +build-base

    ENV TARGET_ARCH=armhf

    RUN dpkg --add-architecture ${TARGET_ARCH}
    RUN apt-get update && \
        apt-get install -y \
            gcc-arm-linux-gnueabihf \
            g++-arm-linux-gnueabihf \
            zlib1g-dev:${TARGET_ARCH} \
            libmosquitto-dev:${TARGET_ARCH}

    RUN BRANCH=$(git rev-parse --abbrev-ref HEAD) \
        COMMIT=$(git rev-parse --short HEAD) \
        CTIME=$(date +%s) \
        cmake -DCMAKE_TOOLCHAIN_FILE=$(pwd)/cmake-toolchains/eabihf.cmake -S .
    RUN make # VERBOSE=1
    SAVE ARTIFACT bin

    WORKDIR nfpm
    RUN mkdir -p build
    RUN nfpm package -p deb -t build/ticktock_${TICKTOCK_VERSION}_${TARGET_ARCH}.deb
    RUN nfpm package -p rpm -t build/ticktock_${TICKTOCK_VERSION}_${TARGET_ARCH}.rpm
    SAVE ARTIFACT build

build-docker:
    FROM bitnami/minideb:latest

    COPY +build-amd64/bin/tt /usr/bin/tt
    COPY +build-amd64/bin/tt_env /usr/bin/tt_env

    EXPOSE 6180/tcp
    EXPOSE 6181/tcp
    EXPOSE 6181/udp
    EXPOSE 6182/tcp
    EXPOSE 6183/tcp

    COPY ./docker/limits.conf /etc/security/
    RUN mkdir -p /opt/ticktock/scripts
    COPY ./docker/docker-entrypoint.sh /opt/ticktock/scripts/

    ENTRYPOINT ["/opt/ticktock/scripts/docker-entrypoint.sh"]

    SAVE IMAGE ticktock:latest
