# animeloop-cli
Anime loop video command line tool.


### Install via Arch Linux User Repository (aur)
#### with [yaourt](https://wiki.archlinux.org/index.php/Yaourt)
``` Shell
yaourt -S animeloop-cli
```


### Install Manually

#### Modules Init

``` Shell
git submodule init
git submodule update
```


#### Dependencies

##### macOS

``` Shell
# OpenCV
brew tap homebrew/science
brew install opencv3 --with-contrib --with-ffmpeg --c++11

# Boost
brew install boost

# FFmpeg
brew install ffmpeg
```

##### Ubuntu (obsoleted)
``` Shell
# OpenCV
sudo apt-get -y install libopencv-dev build-essential cmake git libgtk2.0-dev pkg-config python-dev python-numpy libdc1394-22 libdc1394-22-dev libjpeg-dev libpng12-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev libtbb-dev libqt4-dev libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils unzip

mkdir opencv
cd opencv

wget https://github.com/Itseez/opencv/archive/3.2.0.zip -O opencv-3.2.0.zip
unzip opencv-3.2.0.zip
cd opencv-3.2.0

mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_QT=ON -D WITH_OPENGL=ON ..

make -j $(nproc)
sudo make install

sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig

## uninstall opencv
sudo apt-get remove opencv

# Boost
sudo apt-get install libboost-all-dev

# OpenSSL
sudo apt-get install libssl-dev

# FFmpeg
# unofficial ppa
sudo add-apt-repository ppa:jonathonf/ffmpeg-3
sudo apt update && sudo apt install ffmpeg
```

#### Compile & Build 

##### via Xcode

Open `animeloop-cli.xcodeproj` and run it.

(If you have compiled `opencv` source code by yourself, you may need to edit `Linked Frameworks and Libraries` settings in Xcode project.)

##### via cmake

``` Shell
cd animeloop-cli
cd jsoncpp
python amalgamate.py
cd ..
mkdir build
cd build
cmake ..
make
(make install)
```

### Usage

```Shell


animeloop [OPTION...]

  -h, --help              Show animeloop help
  -v, --version           Show animeloop version
  -i, --input arg         Input video file path
  -o, --output arg        Output video directory path (default:
                          <current dir>)
      --episode arg       Episode name of the source video (default:
                          <filename>)
      --series arg        Series name of the source video  (default:
                          <filename>)
      --min-duration arg  Minimum duration (second) of loop video (default:
                          0.8)
      --max-duration arg  Maximum duration (second) of loop video (default:
                          4.0)
      --cover             Output loop video cover image.


# Example
animeloop -i ~/your-video-file -o ~/Downloads/ --max-duration 4 --min-duration 1.0
```