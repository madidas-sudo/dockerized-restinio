#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. 
#

###
### Build Stage
###
FROM alpine:latest as build
LABEL description="Build container for dockerized-restinio"
RUN apk update && apk add --no-cache \
    autoconf build-base binutils cmake fmt curl zip file gcc g++ git http-parser libgcc libtool linux-headers make musl-dev pkgconfig ninja tar unzip wget

RUN cd /tmp \
    && git clone https://github.com/Microsoft/vcpkg.git \ 
    && cd vcpkg \
    && ./bootstrap-vcpkg.sh

RUN VCPKG_FORCE_SYSTEM_BINARIES=1 ./tmp/vcpkg/vcpkg update
RUN VCPKG_FORCE_SYSTEM_BINARIES=1 ./tmp/vcpkg/vcpkg install boost-asio boost-filesystem boost-program-options boost-test fmt restinio
RUN VCPKG_FORCE_SYSTEM_BINARIES=1 ./tmp/vcpkg/vcpkg install rapidjson
RUN VCPKG_FORCE_SYSTEM_BINARIES=1 ./tmp/vcpkg/vcpkg install json-dto


COPY ./ /dockerized-restinio
WORKDIR /dockerized-restinio
ENV CTEST_OUTPUT_ON_FAILURE=1
RUN mkdir build \
    && cd build \
    && cmake .. -DCMAKE_TOOLCHAIN_FILE=/tmp/vcpkg/scripts/buildsystems/vcpkg.cmake \
    && make \
    && make test

###
### Runtime stage
###
FROM alpine:latest as runtime
LABEL description="Runtime container for dockerized-restinio"

RUN apk update && apk add --no-cache libstdc++
COPY --from=build /dockerized-restinio/build/dockerized-restinio /usr/local/bin/restinio

ENTRYPOINT [ "/usr/local/bin/restinio" ]
EXPOSE 8080

