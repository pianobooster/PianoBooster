![logo](logo/Logotype_horizontal.png)

INTRODUCTION
============

Piano Booster is a MIDI file player that displays the musical notes AND teaches you how to
play the piano. You can play along to any track in the midi file and PianoBooster will
follow YOUR playing. PianoBooster makes sight reading fun!

Piano Booster is a free (Open Source) program available from: <https://github.com/captnfab/PianoBooster/>

Original work by Louis J. Barman can be found here: <http://pianobooster.sourceforge.net/>

A huge thanks to him and to all the contributers.

WINDOWS
=======

To run Piano Booster you need a MIDI Piano Keyboard and a MIDI interface for the PC. (If you
don't have a MIDI keyboard you can still try out PianoBooster using the PC keyboard, 'x' is
middle C - but a MIDI piano is recommend).

To hear the music you will need a General Midi compatible sound synthesizer. Unfortunately
the "Microsoft GS Wavetable software synthesizer" that comes with Windows XP introduces an
unacceptable delay (latency) between pressing a note and hearing the sound. For this reason
the use of "Microsoft GS Wavetable software synthesizer" is not recommend. Please see the
forum for possible solutions.


macOS
=====

To run Piano Booster you need a MIDI Piano Keyboard and a MIDI-USB interface if the keyboard
does not have a USB interface. (If you don't have a MIDI keyboard you can still try out
PianoBooster using the computer's keyboard, 'x' is middle C).

To hear music from the computer you will need a General Midi compatible sound synthesizer.
While there are many possible choices for the Mac (Timidity, FluidSynth or an external MIDI
Synth), the easiest way is to use SimpleSynth, which can be obtained for free (MIT license)
from: <http://notahat.com/simplesynth>

This must be started before Pianobooster, and also allows to load custom Soundfonts.

Double Click on the DMG file containing PianoBooster, then just drag the pianobooster.app
file wherever you wish (/Application folder is suggested).

[pianobooster-mac-0.6.5-alpha1.dmg](https://github.com/chrisballinger/PianoBooster/releases/tag/v0.6.5-alpha1) (not official)

LINUX and BSD UNIX
==================

To run Piano Booster you need a MIDI Piano Keyboard and a MIDI interface for the PC. (If you
don't have a MIDI keyboard you can still try out PianoBooster using the PC keyboard, 'x' is
middle C - but a MIDI piano is recommend).

To hear music from the computer you will need a General Midi compatible sound synthesizer.

On Linux to start fluidsynth type the following on a terminal window.

```
fluidsynth -g 1.0 -a alsa -o midi.alsa_seq.id=fs2 /usr/share/sounds/sf2/FluidR3_GM.sf2
```
An alternatively you can used TiMidity by type the following on a terminal window.

```
timidity -iA -B2,8 -Os -EFreverb=0
```

MIDI FILES
==========

To run PianoBooster you will need some Midi files preferably with right and left piano
parts on channels 4 and 3. Some high quality demo MIDI files that are compatible with
PianoBooster are available in doc/courses/*/ directories.

INSTRUCTIONS
============

The MIDI input device is used to connect your piano keyboard to the PC. First ensure that your
piano keyboard is connected up to the PC using a MIDI USB adaptor (or through a PC sound card).
Once the Piano Booster application starts up you must first setup the midi input and midi
output interface from the Setup menu. Next open a midi file ".mid" or a karaoke ".kar" file
using File/Open from the Piano Booster menu. Now choose the skill level, if you want to
just listen to the midi music  - select 'listen', to play along with a midi keyboard with
the music following your playing - select 'follow you'. Finally click on the Play icon to
start.

It is recommended that you shut down all other programs while running Piano Booster so that
the scrolling notes move smoothly across the screen.

TRANSLATIONS
============

PianoBooster isn't translated in your language? Help us, and we will gladly ship
new translations with it.

LICENSE
=======

Piano Booster is fully copyrighted by the author and all rights are reserved.

PianoBooster is free software (Open Source software): you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

PianoBooster is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License in the file "gplv3.txt" or from the web site
<http://www.gnu.org/licenses/> or [gplv3.txt](gplv3.txt).

The Piano Booster Documentation and Music are released under the Creative Commons License (CC-BY).

[PianoBooster License](license.txt)

SOURCE CODE
===========

Download and uncompress the archive file below and then follow the instructions in
the *BUILD.md* file.

[PianoBooster releases](https://github.com/captnfab/PianoBooster/releases)

As part of GNU General Public License the source code for PianoBooster is available from
the GitHub site  <https://github.com/captnfab/PianoBooster>. See the [BUILD.md](BUILD.md) for
instructions on how to build the source code.

DEBUGGING
=========

To get a backtrace, build Piano Booster with DEBUG option

type:

`cmake -DCMAKE_BUILD_TYPE=Debug .`

type:

`gdb ./pianobooster`

then:

`run`

then:

`bt`

DOCUMENTATION
=============

[Documentation](doc/README.md)

[Changelog](Changelog.txt)

BUGTRACKER
==========

[Issues](https://github.com/captnfab/PianoBooster/issues)

CONTRIBUTING
============

PianoBooster is community developing project since 2018. You're welcome with patches:

[Pull Requests](https://github.com/captnfab/PianoBooster/pulls)

We have many [issues](https://github.com/captnfab/PianoBooster/issues) always.

We are looking for musicians to make music for pianobooster. It can be educational courses or other melodies: [music sources](music/src). For more information look at [README.md](doc/courses/README.md) and [faq.md](doc/faq.md).

PLATFORMS
=========

We support Linux, BSD Unix and Windows platforms. We do not have macOS developers at the moment.
