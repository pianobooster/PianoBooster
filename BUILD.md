# Build instructions for Linux and BSD Unix

Ensure that the following packages are installed:
`build-essential`, `cmake`, `pkg-config`, `libftgl-dev`, `qtbase5-dev`, `qttools5-dev`, `librtmidi-dev` , `libfluidsynth-dev`, `fluid-soundfont-gm`



To generate a project makefile using CMake, create a build folder
and then run CMake from it:
```
$ mkdir build
$ cd build
$ cmake ..
```

To compile the PianoBooster source code type:

`make -j$(nproc)`

The pianobooster binary executable is now in a new build subdirectory.
PianoBooster is designed to run in place so you can type `build/pianobooster` to start running the program.

(Optional) If you wish to install PianoBooster as root type:

`make install`

There is no make uninstall command but the files that have been installed are listed in a file called `install_manifest.txt` which can then be used to manually delete the installed files.

To build a debug version create a directory called "debug" and change to that dir and then type:

`cmake -DCMAKE_BUILD_TYPE=Debug ..`


# Build instructions for macOS

Install the latest Xcode (from Apple Developer Connection, free registration required).

Install CMake and QT libraries via Homebrew:

`$ brew install cmake qt5 ftgl pkg-config fluid-synth`

To generate the project makefile first create a `build` directory
and then from that directory type:

`cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"`

To compile the PianoBooster source code type:

`make`

To copy the language translations into the build directory type:

`make install-translations`


To make a self contained application bundle use QT's macdeployqt tool (included in QT).

`$(brew --prefix qt)/bin/macdeployqt build/pianobooster.app -dmg`



# Build instructions for Windows

To compile in Windows install the Open Source version of Qt and CMake.
When installing Qt select the option to download and install the MinGW compiler. Open the Qt Command Prompt and change to the "PianoBooster" source code directory and then create a `build` directory and then from that directory type:

`cmake.exe -G "MinGW Makefiles" ..`

Once this is completed type:

`make`

Or alternatively you can install QtCreator and then open the `CMakeLists.txt`.

# Build instructions for Raspberry PI OS and other ARM Devices

Building on these devices requires QT compiled with desktop OpenGL support. OpenGL/ES is not supported at this time. If after compiling and running PianoBooster you see messages concerning OpenGL/ES like this:

```
QGLWidget::renderText is not supported under OpenGL/ES
```

it means that your version of QTOpenGL will not support PianoBooster. To fix this you must obtain an alternate package from your distribtion or compile your own version of QT with desktop opengl support. For instructions on building QT5, please read the [QT5 Linux Building Instructions](https://doc.qt.io/qt-5/linux-building.html)

Use the following command on the configure step:
```
./configure -opensource -opengl desktop
```

After building and installing QT, you can build PianoBooster with the above Linux and BSD Unix steps.  If you installed your QT in a prefix then you can set `$LD_LIBRARY_PATH` to `$QTPREFIX/lib` and then run it.

# Building an AppImage

For compatility reasons, it is recommended to build the AppImage on the oldest still supported distribution that you can assume users still use.

This can be accomplished by first installing packages specified in the Linux and BSD Unix steps above. You will also need [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy). You can use the appimage or build it yourself. To build the PianoBooster appimage run:

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
$ make
$ make install DESTDIR=AppDir

$ linuxdeploy --appdir=AppDir --plugin qt --output appimage
or (if built separately)
$ linuxdeploy --appdir=AppDir && linuxdeploy-plugin-qt --appdir=AppDir && linuxdeploy-plugin-appimage --appdir=AppDir
```

# Build options

Using `cmake` without any flags defaults to the recommended build options.
However the following build options below can be changed by using the `-D` flag to `cmake`.

**WITH_INTERNAL_FLUIDSYNTH:** build with an internal FluidSynth sound generator [Default:ON]

**USE_BUNDLED_RTMIDI:**  Build with bundled rtmidi (for older distributions only) [Default: OFF]

**USE_FTGL:** Build with ftgl for notes localization [Default:ON]

**USE_SYSTEM_FONT:** Build with system font [Default: OFF]

**USE_JACK:** Build with Jack. The use of JACK is not required other than for BSD Unix. [Default: OFF]

**DATA_DIR**: Build with specified data directory; [Default:"share/games/pianobooster"]

**NO_LANGS**: Do not install languages; [Default: OFF]

**NO_DOCS**: Do not install documents [Default: OFF]

**NO_LICENSE**: Do not install license [Default: OFF].

**NO_CHANGELOG**: Do not install changelog [Default: OFF].

**WITH_MAN**: Install man page [Default: OFF].
