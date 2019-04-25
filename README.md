# animeloop-cli
![Animeloop Logo](https://animeloop.org/files/animeloop.gif)

Animeloop command line tool. The goal of this project is to automatically find and cut out the looping fragments in Japanese anime. For details, please see my [technical report(English)](https://animeloop.org/files/paper/technical_report_en.pdf), [technical report(Chinese)](https://animeloop.org/files/paper/technical_report_zh.pdf).

_This is an unoffical MSVC port._

## Note for MSVC version

### Why?

As I need `animeloop-cli` to run on Windows, so I tried to build it by myself.

Unfortunately, when I built it with MinGW under MSYS2, GCC couldn't compile `child_process.cpp`, as it uses `fork()` that only works on POSIX (or say *nix).

Then I tried to build it with MSYS under MSYS2, which is an Cygwin port -- which simulates POSIX functions -- after resolving linking problems, it compiles successfully, but when it runs, it shows runtime error `while loading shared libraries: ?`, and I don't know what is `?`.

Then I tried Cygwin, after grabbing all the dependencies with the messy installer (sorry but I think package manager like `apt` or `pacman` is better) and start to compile, it shows some compile target errors, and I'm not sure what's up.

So I gave it up, and made a Microsoft Visual C++ port.

### What's changed?

- Instead of using POSIX `fork()`, it uses Windows `CreateProcess()` to create child process, so it's not available to build on *nix.

- Use Windows CLI `where` to detect FFmpeg.

- Show FFmpeg convert status when resizing video.

### How to build?

Checkout _Installation_ section, the Windows steps are appended after Arch Linux (and you won't like it).

Or you can download the binary built by myself from [GitHub release page](https://github.com/ccloli/animeloop-cli-msvc/releases).

## Installation

### Install via Arch Linux User Repository (aur)

> This is the original release, not the MSVC one.

#### with [yaourt](https://wiki.archlinux.org/index.php/Yaourt)
``` Shell
yaourt -S animeloop-cli
```

### Download prebuilt binary for Windows

Checkout [GitHub release page](https://github.com/ccloli/animeloop-cli-msvc/releases), and download FFmpeg 3.x binary from https://ffmpeg.zeranoe.com/ or build FFmpeg by yourself. 

### Install manually

#### Modules Init

``` Shell
git submodule init
git submodule update
```


#### Dependencies

##### macOS

> Not supported for this fork (except Visual Studio for macOS does some trick to support Windows API?)

##### Debian/Ubuntu

> Not supported for this fork

##### Arch Linux

> Not supported for this fork

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

#### Compile & Build 

##### Xcode

> Not supported for this fork

##### CMake

``` Shell
cd animeloop-cli
mkdir build && cd build
cmake ..
make animeloop-cli
```

##### CMake-GUI and Visual Studio

- CMake-GUI
	* Create folder `build` under `animeloop-cli` folder as `Target` folder
	* Set `Source` and `Target` folder
	* Click `Configure` button, select Visual Studio 2015
	* Resolve missing parameters
	* Click `Genereate` button
	* If everything is okay, open the `sln` file under `build` folder

- Visual Studio
	* Select `animeloop-cli` project, and start building
	* If link directories or include directories specificed in CMake is not included, you should config them manually. Here are my configuration, you can change it to your exact folder:
		```
		Include directories: Y:\opencv\build\include;Y:\hdf5\HDF_Group\HDF5\1.10.5\include;Y:\jsoncpp\include;Y:\boost_1_64_0;$(IncludePath)
		Library directories: Y:\opencv\build\x64\vc14\lib;Y:\hdf5\HDF_Group\HDF5\1.10.5\lib;Y:\jsoncpp\lib;Y:\boost_1_64_0\lib64-msvc-14.0;$(LibraryPath)
		```
	* If you got `C2001` error or it shows encoding problem, convert all files to UTF-8 with BOM, or set project encoding to Unicode (but may still meet encoding problem when running on some code page like CP936).

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

> You should grab a FFmpeg 3.x binary from https://ffmpeg.zeranoe.com/ to make it works.

> You should download OpenH264 1.7.0 library file from https://github.com/cisco/openh264/releases manually.

> You may also need Visual C++ 2015 Redistributable.

## License

This project is available under the MIT License. See the LICENSE file for more info.