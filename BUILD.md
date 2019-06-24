# Linux

   Ensure that the following packages 'cmake', 'libqt5-dev', 'libasound2-dev' and
   'build-essential" are installed.

   Then type

   ```cmake .```

   ```make```

   Finally as root type

   ```make install -C build```

   (The pianobooster binary executable is now in the "build" subdirectory.)

   To build a debug version create a dir called "debug" and change to that dir and then type

   ```cmake -DCMAKE_BUILD_TYPE=Debug .```

   (Alternatively you can use qmake followed by make.)

   See [DEB](pkgs/deb) for more details.

   See [RPM](pkgs/rpm/pianobooster.spec) for more details.

   See [ARCH](pkgs/arch/PKGBUILD) for more details.

   If you make changes to the source code then please post details.

# macOS

   Install latest Xcode (from Apple Developer Connection, free registration required).

   Install CMake and QT libraries via Homebrew:

   ```$ brew install cmake qt5```

   Generate XCode project file via CMake:

   ```$ cmake -G Xcode . -DCMAKE_PREFIX_PATH=$(brew --prefix qt)```

   Open the project file in XCode, set whatever options you like (universal or single architecture,
   debug or release etc.) and compile.

   To make a self contained application bundle use QT's macdeployqt tool (included in QT 4.5.0).

   If you make changes to the source code then please post details.

# Windows

   To compile in Windows install the Open Source version of Qt and CMake and optionally Geany.
   When installing Qt select the option to download and install the MinGW compiler. Open the
   Qt Command Prompt and then change to the "PianoBooster" directory and then type the
   command below.

   ```"C:\Program Files\CMake 2.6\bin\cmake.exe" -G "MinGW Makefiles" .```

   Once this is completed type

   ```make```

   Or alternatively you can install QtCreator and then open the pianobooster.pro

   If you make changes to the source code then please post details.
