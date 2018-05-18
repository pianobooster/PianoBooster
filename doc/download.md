# Download PianoBooster

## Requirements
  
To run Piano Booster you need a MIDI Piano Keyboard and a MIDI interface for the PC. (If you
don't have a MIDI keyboard you can still try out PianoBooster using the PC keyboard, 'x' is
middle C -- but a MIDI piano is recommend).

To hear the music you will need a General Midi compatible sound synthesizer. Unfortunately
the "Microsoft GS Wavetable software synthesizer" that comes with Windows introduces an
unacceptable delay (latency) between pressing a note and hearing the sound. For this reason
the use of "Microsoft GS Wavetable software synthesizer" is not recommend. Please see the
forum section of this website for possible solutions.

## MIDI files

To run PianoBooster you will need some Midi files preferably with right and left piano parts on channels 4 and 3.
Some high quality demo MIDI files that are compatible with
PianoBooster are available [here](http://www.pianobooster.pwp.blueyonder.co.uk/)

## Executable for Windows

There is now an installer/uninstaller for windows. Just download and run this setup program.

[PianoBoosterInstall-0-6-4.exe](https://sourceforge.net/projects/pianobooster/files/pianobooster/0.6.4/PianoBoosterInstall-0-6-4.exe/download)

## Executable for The Mac

Piano Booster has been tested with [SimplSynth](http://notahat.com/simplesynth)
for the Mac which can be obtained for free (MIT license).

Version 0.6.4 for the Mac is coming soon in the meantime here is the previous release.
A universal binary for the Mac (Thanks to Christian).

[pianobooster-mac-0.6.2.dmg](https://sourceforge.net/projects/pianobooster/files/pianobooster/0.6.2/pianobooster-mac-0.6.2.dmg/download)

## Packages for Linux

*A Ubuntu Package* for Piano Booster for the current version 0.6.4 is available [here](https://launchpad.net/~racb/+archive/extra)

Download and install the .deb package that corresponds to your architecture using your favourite package manager.

Note: In Ubuntu 9.04 there is an issue with the Intel drivers which that means PianoBooster
performs very badly if you have Intel graphic chips.  Upgrading to Ubuntu 9.10 fixes this problem.

*A Fedora Package* for Piano Booster is available as part of the distribution see:
[http://www.rpmfind.net](http://www.rpmfind.net) then search for *pianobooster*

*Note:* The packages provided with distributions may not exist or are out of date,
in which case you will have to compile from the source code.
However compiling from source is quite easy on Linux, see the instructions
in the section below.

## Source Code for all platforms

Download and uncompress the archive file below and then follow the instructions in
the *BUILD.txt* file.

[pianobooster-src-0.6.4b.tar.gz](https://sourceforge.net/projects/pianobooster/files/pianobooster/0.6.4b/pianobooster-src-0.6.4b.tar.gz/download)

*Compiling on Linux:* You need to have the following packages installed *cmake*,
*libqt4-dev*, *libasound2-dev*, and the *build-essential* packages. Then in the
*pianobooster/build* directory type `cmake ../src`, then type `make`, followed
by `sudo make install`. Finally type `sudo update-desktop-database` to get *open
with* working in Nautilus. (Version 0.6.4b fixes a problem on Ubutnu 10.4 Lucid
Lynx).

*Compiling on Windows:* The easiest way to compile on windows download and run LGPL / Free version of *Qt SDK: Complete Development Environment*.
