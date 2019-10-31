FROM 	ubuntu:16.04

RUN apt-get update \
&&  apt-get upgrade -y \
&&  apt-get install -y --no-install-recommends python python-dev python-pip build-essential cmake git pkg-config libjpeg8-dev libtiff5-dev libjasper-dev libpng12-dev libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libatlas-base-dev gfortran libavresample-dev libgphoto2-dev libgstreamer-plugins-base1.0-dev libdc1394-22-dev  \
&&  cd /opt \
&&  git clone https://github.com/opencv/opencv.git \
&&  cd opencv \
&&  git checkout 3.4.0 \
&&  mkdir build \
&&  cd build \
&&  cmake \
      -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D INSTALL_C_EXAMPLES=OFF \
      -D INSTALL_PYTHON_EXAMPLES=OFF \
      -D BUILD_EXAMPLES=OFF /opt/opencv \
&&  make -j $(nproc) \
&&  make install \
&&  ldconfig \
&&  rm -rf /opt/opencv*

# Install animeloop-cli required packages
RUN apt-get install -y cmake git pkg-config \
&&  apt-get install -y libboost-all-dev \
&&  apt-get install -y ffmpeg x264 x265 \
&&  apt-get install -y libjsoncpp-dev \
&&  apt-get clean && rm -rf /var/lib/apt/lists/*

# Build animeloop-cli
RUN git clone https://github.com/moeoverflow/animeloop-cli.git \
&&  cd animeloop-cli \
&&  git submodule init && git submodule update \
&&  mkdir build && cd build \
&&  cmake .. \
&&  make animeloop-cli \
&&  cp /animeloop-cli/build/animeloop-cli /usr/bin \
&&  chmod +x /usr/bin/animeloop-cli

ENTRYPOINT [ "/usr/bin/animeloop-cli"]