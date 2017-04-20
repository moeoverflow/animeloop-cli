# animeloop-cli
Anime loop video command line tool.

### Installation

#### Git Submodules Init

```Shell
$ git submodule init
$ git submodule update
```

#### macOS 

Just run in Xcode.

#### Cross-Platform

CMake is not available currently.

## Usage

```Shell
$ animeloop -i ~/your-video-file -o ~/Downloads/ --max-duration 4 --min-duration 1.0
```

## Dependencies

* Boost
* OpenCV 3

## Thanks

* cxxopts