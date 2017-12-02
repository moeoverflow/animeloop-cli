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

# compiler
sudo apt-get install build-essential
# required
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
# optional
# sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev

mkdir opencv
cd opencv

wget https://github.com/opencv/opencv/archive/3.3.1.zip -O opencv-3.3.1.zip
unzip opencv-3.3.1.zip
cd opencv-3.3.1

mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..

make -j $(nproc)

# Boost
sudo apt-get install libboost-all-dev

# FFmpeg
sudo apt-get update && sudo apt-get install ffmpeg

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
anime loop video generator.

Usage:
  animeloop [OPTION...]

  -h, --help              Show animeloop help
  -v, --version           Show animeloop version
  -i, --input arg         Input video file path
  -o, --output arg        Output video directory path (default: .)
      --title arg         Title name of the source video (default:
                          <filename>)
  -t, --thread arg        Program run in n threads. (default: <cpu core
                          number>)
      --min-duration arg  Minimum duration (second) of loop video (default:
                          0.6)
      --max-duration arg  Maximum duration (second) of loop video (default:
                          6.0)
      --cover             Output loop video cover image.


# Example
./animeloop-cli -i ~/your-video-file --max-duration 4 --min-duration 1.0 --cover -o ~/Downloads/
```