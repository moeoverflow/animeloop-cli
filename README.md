# animeloop-cli
Anime loop video command line tool.

## Installation

### Install via Arch Linux User Repository (aur)
#### with [yaourt](https://wiki.archlinux.org/index.php/Yaourt)
``` Shell
yaourt -S animeloop-cli
```



### Install manually

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

# Jsoncpp
brew install jsoncpp
```

##### Debian/Ubuntu

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

sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig

## uninstall opencv
sudo apt-get remove opencv

# Boost
sudo apt-get install libboost-all-dev

# FFmpeg
# unofficial ppa
sudo add-apt-repository ppa:jonathonf/ffmpeg-3
sudo apt update && sudo apt install ffmpeg

# Jsoncpp
sudo apt-get install libjsoncpp-dev
```

##### Arch Linux

```Shell
# Boost
pacman -S boost

# OpenCV
pacman -S opencv
pacman -S hdf5

# Jsoncpp
pacman -S jsoncpp
```

#### Compile & Build 

##### Xcode

Open `animeloop-cli.xcodeproj` and run it.

(if you compile opencv source code by yourself, you maybe need edit `Linked Frameworks and Libraries` settings in Xcode project.)

##### CMake

``` Shell
cd animeloop-cli
mkdir build && cd build
cmake ..
make animeloop-cli
```

## Usage

```Shell
Anime loop video generator.

Usage:
  animeloop [OPTION...]

  -h, --help              Show animeloop help
  -v, --version           Show animeloop version
  -i, --input arg         Input video file path
  -o, --output arg        Output video directory path (default: .)
      --title arg         Title name of the source video (default:
                          <filename>)
      --min-duration arg  Minimum duration (second) of loop video (default:
                          0.6)
      --max-duration arg  Maximum duration (second) of loop video (default:
                          6.0)
      --cover             Output loop video cover image.


# Example
animeloop -i ~/your-video-file -o ~/Downloads/ --max-duration 4 --min-duration 1.0
```