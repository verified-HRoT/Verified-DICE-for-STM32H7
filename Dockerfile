FROM zhetao/dice-star:artifact

RUN sudo apt-get -qq update

# Dependencies for this repo
RUN sudo apt-get -y install \
    build-essential \
    cmake \
    ninja-build \
    libgtest-dev \
    liblz4-dev \
    zip \
    unzip \
    git \
    tar \
    wget

# Get GCC-ARM toolchain

RUN GCC_ARM=gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2 && \
    wget -q "https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2?revision=c34d758a-be0c-476e-a2de-af8c6e16a8a2?product=GNU%20Arm%20Embedded%20Toolchain%20Downloads,64-bit,,Linux,8-2019-q3-update" -O $GCC_ARM && \
    tar -C ~ -xf $GCC_ARM

RUN sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 100 && \
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 100

# The C code is generated in the `${DICESTAR_HOME}/dist/` folder

ENV FIRMWARE_DIR=${DICESTAR_HOME}/experiment
RUN mkdir -p ${FIRMWARE_DIR}
COPY --chown=dicestar . ${FIRMWARE_DIR}
WORKDIR ${FIRMWARE_DIR}

# Make sure all submodules are cloned properly
RUN git submodule update --init --recursive

# Copy DICE* code.
RUN ${FIRMWARE_DIR}/copy-dice-star-code.sh && \
    rm -rf ${FIRMWARE_DIR}/copy-dice-star-code.sh

# build
# RUN ${FIRMWARE_DIR}/build.sh && \
    # rm -rf ${FIRMWARE_DIR}/build.sh

# wipe source code
# RUN rm -rf ${FIRMWARE_DIR}/3rdparty ${FIRMWARE_DIR}/cmake ${FIRMWARE_DIR}/patches ${FIRMWARE_DIR}/src ${FIRMWARE_DIR}/.git*

WORKDIR ${DICESTAR_HOME}

CMD /bin/bash
