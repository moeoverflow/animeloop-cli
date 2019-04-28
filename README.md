# animeloop-cli
![Animeloop Logo](https://animeloop.org/files/animeloop.gif)

Animeloop command line tool. The goal of this project is to automatically find and cut out the looping fragments in Japanese anime. For details, please see my [technical report(English)](https://animeloop.org/files/paper/technical_report_en.pdf), [technical report(Chinese)](https://animeloop.org/files/paper/technical_report_zh.pdf).

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

# FFmpeg !!!! ffmpeg version 3.x required !!!!
sudo add-apt-repository ppa:jonathonf/ffmpeg-3
sudo apt update && sudo apt install ffmpeg libav-tools x264 x265
## downgrade FFmpeg
sudo apt install ppa-purge && sudo ppa-purge ppa:jonathonf/ffmpeg-3

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

##### Windows MinGW

```shell
# - MinGW build require tool MSYS2, you sould download and install it first, then use pacman in MSYS2 Enviroment
#
# - Assume you're building to mingw64, if you'd like to build to mingw32, replace `mingw-w64-x86_64` with `mingw-w64-i686`
#
# - Some packages are updated and not compatible, so some of the steps are using `pacman -U` to install specific version
#   packages with USTC Mirrors, you can change `https://mirrors.ustc.edu.cn/msys2/mingw/x86_64/` to other sources.

# Build deps
pacman -S libintl mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make

# Boost
# - It seems that when using Boost 1.66+ will cause build fail
pacman -U https://mirrors.ustc.edu.cn/msys2/mingw/x86_64/mingw-w64-x86_64-boost-1.64.0-3-any.pkg.tar.xz

# OpenCV
pacman -U https://mirrors.ustc.edu.cn/msys2/mingw/x86_64/mingw-w64-x86_64-opencv-3.4.3-2-any.pkg.tar.xz
pacman -S mingw-w64-x86_64-hdf5

# Jsoncpp
# - JSONCPP comes with cmake, so if you've install cmake manually, you're not needed to install JSONCPP again
pacman -S mingw-w64-x86_64-jsoncpp jsoncpp-devel

# FFmpeg
# - Some dll files are missing in MSYS2 MinGW64 FFmpeg package, so you should download it from other builds
# - You can also download it later, as it's not build dependency (but runtime dependency)
# pacman -U https://mirrors.ustc.edu.cn/msys2/mingw/x86_64/mingw-w64-x86_64-ffmpeg-3.4.2-1-any.pkg.tar.xz
wget https://ffmpeg.zeranoe.com/builds/win64/static/ffmpeg-3.4.2-win64-static.zip
unzip -p ffmpeg-3.4.2-win64-static.zip ffmpeg-3.4.2-win64-static/bin/ffmpeg.exe > /mingw64/bin/ffmpeg.exe
```

##### Windows via Microsoft Visual Studio

- Download and install [Microsoft Visual Studio](https://visualstudio.microsoft.com/)
    
> Will uses Visual Studio 2015 Enterprise (VC14) as example for below.
    
- Download [CMake-GUI](https://cmake.org/download/)

- Download [Boost prebuilt library](https://sourceforge.net/projects/boost/files/boost-binaries/) for VC14
	
> You may need a older version (< 1.66.0) in case of some unknown bug.
	
- Download [OpenCV prebuilt library](https://opencv.org/releases/) for Windows
	
> OpenCV should be 3.2+ but not 4.x
	
- Download [HDF5 prebuilt library](https://www.hdfgroup.org/downloads/hdf5) for VC14
	> You may need an account to download the file, you can register for free or build your own.
	> If you don't want to install the msi package, run `msiexec /a drive:\path\to\file.msi /qb TARGETDIR=drive:\path\to\extract`

- Download [JSONCPP prebuilt library](http://access.osvr.com/binary/deps/jsoncpp) for VC14

> You should grab a FFmpeg 3.x binary from https://ffmpeg.zeranoe.com/ to make it works.

> You should download OpenH264 1.7.0+ library file from https://github.com/cisco/openh264/releases manually.

> If you want to use it on Windows, always use backslash to set file path like `D:\path\to\video.mp4`, even under MSYS2 or Cygwin, or boost cannot find the specific path.

> You may also need Visual C++ 2015 Redistributable.

#### Compile & Build

##### Xcode

Open `animeloop-cli.xcodeproj` and run it.

(if you compile opencv source code by yourself, you maybe need edit `Linked Frameworks and Libraries` settings in Xcode project.)

##### CMake (*nix)

``` Shell
cd animeloop-cli
mkdir build && cd build
cmake ..
make animeloop-cli
```

##### MSYS2 + MinGW + CMake (Windows)

``` Shell
cd animeloop-cli
mkdir build && cd build
cmake -G "MinGw Makefiles" ..
mingw32-make.exe animeloop-cli
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

## Special Thanks

* [@ccloli](https://github.com/ccloli) for Windows build support

## License

This project is available under the MIT License. See the LICENSE file for more info.