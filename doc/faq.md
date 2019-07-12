# Frequently Asked Questions

## Where are the music files?

Piano Booster uses MIDI music files (.mid) or karaoke files (.kar) to play music.

See [this thread](http://piano-booster.2625608.n2.nabble.com/Pieces-of-music-that-I-have-enjoyed-playing-please-add-to-this-list-td1600345.html)
on the forum which list useful source of midi files on the net.
Please add to this list if you find any pieces of music that you enjoy playing.

## Community Favorites MIDI

Google search: Taylor's Traditional Tune Book, and download the "English Tunes". They are only single melody to play with quite good MIDI arrangements. Barry is quite fond of key changes which make it quite challenging. Especially "I Saw Three Ships."

[Bach collection](https://www.mutopiaproject.org/cgibin/make-table.cgi?collection=bachgb&preview=1) and others from repository of sheet music - [Mutopia Project](https://www.mutopiaproject.org/)

[Miracle Piano Tutor](http://piano-booster.2625608.n2.nabble.com/attachment/4582249/0/teachmid.tar.gz)

## CC and MIDI: What Songs are Legal to Share

Piano Booster will only distribute MIDI files submitted to the project under the Creative Commons (CC) License.

MIDI files created by hand constitute "original work". Original work can be released under the CC license. (see [forum discussion](http://piano-booster.2625608.n2.nabble.com/Creating-music-for-PianoBooster-using-MMA-Everyone-can-help-td4167350.html#a4167350))

MIDI files copied from existing works, (either digital copies, format-shifted copies, or 'recording' the playback output of works) are subject to the copyright laws under which the original works were released.

... (other cases? please add relevant links, discussion)

## Making MIDI

Although MIDI files can be generated in several ways, this guide covers a method that also produces sheet music and accompaniment.
The general process: write "abc notation" of music (ABC file); convert ABC file to Solo MIDI and Sheet Music; write accompaniment "hints" (MMA file); convert solo MIDI and MMA file to final MIDI (ready to practice playing).

### ABC Notation

The ABCPlus notation describes the notes to be played by the right and left hands in plain text ascii-character file.

[Wiki](http://en.wikipedia.org/wiki/Abc_notation)

[Site](http://abcnotation.com/)

[The ABC Plus Project](http://abcplus.sourceforge.net/)

[Examples](http://abcnotation.com/examples)

### ABC to MIDI Conversion

#### Generating MIDI file

The ABC file is converted to a MIDI file using the abc2midi tool. This creates a MIDI file with right and left hand parts.

#### Generating sheet music

The ABC file can also be converted to a sheet music image with the abc2mtex tool.

### Auto-generated Accompaniment

The MMA software can greatly simplify creating accompanying "background" music.

[Musical MIDI Accompaniment](http://www.mellowood.ca/mma/)

## Open a MIDI File

Select File -> Open from PianoBooster main menu and then choose the .mid file or the .kar file. If you put all your MIDI files in adjacent directories then each directory will appear as books that can be accessed using the -book drop down list- and then each MIDI file can be accessed from the song drop down list.

## Listen to the MIDI music

When learning a new piece of music it is recommended that you first listen to the piece of music to hear how it should sound. Set the /skill level to listen/ and then click the /play stop button icon/. To stop the music click the /play/stop button again. To play the music from the beginning click on the /play from start button/.

## Play along with the MIDI music

Piano Booster has been specially designed so that you can play along with the MIDI music using a piano keyboard. Ensure that MIDI input is correctly set up. You can choose which hand to play select 'right', 'both' or 'left' from the hands selection. If you choose to play just a single part then Piano Booster will automatically play the other part for you. There are two different ways of playing along the 'follow you' mode and the 'play along' mode. When learning a new piece of music the follow you mode makes it much easier but you should progress to the 'play along' method as this ensure that you play with the correct timing.

## Playing the piano with the music 'following your' playing

Select the 'follow you' option and choose which hand you want play and then click the play button to start the accompaniment playing.
The music will start playing and at the same time you should playing along to the notes shown on the screen.

##  Connecting MIDI Hardware

While Piano Booster can take input from the computer keyboard, it is recommended to use a MIDI capable keyboard or piano. This section assumes you are using a keyboard or piano with either a USB MIDI port or MIDI input/output ports with a MIDI/USB adapter.

MIDI or Musical Instrument Digital Interface allows electronic musical equipment to communicate with each other. MIDI works like a pipeline; data (the music notes) "flows" in one direction. Every MIDI device port is labeled as either IN/input or OUT/output. The Output of the first device goes to an Input of the next device.

If your keyboard has a USB MIDI port, then a standard USB cable to the computer will provide both input from keys and output to the keyboard speakers (if there are speakers).

If the keyboard as MIDI IN/OUT ports, then the OUTPUT port of keyboard must be connected the INPUT of the USB MIDI adapter. To hear the Piano booster output on the keyboard speakers, the OUTPUT of the USB adapter must be connected to the INPUT port on the keyboard.

## How do I hear the sound on Linux, BSD Unix?

To hear the sound Piano Booster needs a MIDI sound generator,
there are two different General Midi sound generators available on Linux, 'fluidsynth' and 'timidity'.

It's recommends to use 'fluidsynth' for BSD Unix.

To install fluidsynth you must install the following packages 'fluidsynth' 'fluid-soundfont-gm' and
'fluid-soundfont-gs' using your favourite package manager

or

Arch Linux: 'fluidsynth' 'soundfont-fluid'.
BSD Unix: 'fluidsynth' 'fluid-soundfont'.

To start the fluidsynth sound generator copy and paste the following command line (one very long command line).

Using ALSA:

`fluidsynth -i -s -g 0.4 -C 0 -R 0 -r 44100 -c 6 -z 128 -l -a alsa -o audio.alsa.device=plughw:0 -o midi.alsa_seq.id=fs /usr/share/sounds/sf2/FluidR3_GM.sf2 /usr/share/sounds/sf2/FluidR3_GS.sf2`

Using JACK:

`fluidsynth -i -s -g 0.4 -C 0 -R 0 -r 44100 -c 6 -z 128 -l -a jack -o midi.alsa_seq.id=fs -j /usr/share/sounds/sf2/FluidR3_GM.sf2 /usr/share/sounds/sf2/FluidR3_GS.sf2`

BSD Unix does not have ALSA, so you should use JACK:

`fluidsynth -i -s -g 0.4 -C 0 -R 0 -r 44100 -c 6 -z 128 -l -a jack -j /usr/local/share/sounds/sf2/FluidR3_GM.sf2 /usr/local/share/sounds/sf2/FluidR3_GS.sf2`

*Note:* Path `/usr/share/sounds/sf2` is Debian, Ubuntu, OpenSuse, Slackware, etc. For Alt Linux, Arch Linux, CentOS, Fedora, Mageia, OpenMandriva, ROSA, etc. you must use path `/usr/share/soundfonts` instead. BSD Unix uses `/usr/local/share/sounds/sf2`.

*Note:* JACK in BSD Unix cannot be started realtime by user, but it starts realtime by default, user does not have permissions. Disabling realtime helps. How to disable realtime or allow user to use realtime please ask your favorite BSD Unix forum.

If you have a fast machine then you can leave out the flags  `-C 0 -R 0 -r 22050` which turns off the reverb, chorus and reduce the sample rate.

The flags `-c 6 -z 128` control the latency try `-c 5 -z 128` for less latency,
but at the risk of audio drop outs.

The flags `-o audio.alsa.device=plughw:0` bypasses the Pulse Audio layer
which caused a lot of latency delay but unfortunately these flags may also
bypass the desktop volume controls.

Flag `-i`: don't read commands from the shell. Flag `-s`: start FluidSynth as a server process. Flag `-g 1`: set the master gain = 1.

Flag `-a alsa` or `-a jack`: the name of the audio driver to use.

Flag `-j`: attempt to connect the jack outputs to the physical ports.

Flag `-l`: don't connect to LASH server.

For more information on running fluidsynth with low latency see
[this page](https://github.com/FluidSynth/fluidsynth/wiki/LowLatency).

On PianoBooster set the Setup/Midi Setup/Midi Output Device: to `FLUID Synth (fs):0`

If you get problems with unsteady playback or the audio cutting out then try closing all other programs,
turning off your wireless network and unplug any network cable.

If you get an error, make sure to have snd-seq-midi kernel module loaded in your
system (sudo modprobe snd-seq-midi).

You might prefer to use qsynth as GUI interface to fluidsynth for easy setting
of Reverbs, chorus effects, etc.

Using 'timidity': [wrapper script and desktop file](https://github.com/captnfab/PianoBooster/tree/master/tools/timidity).

Using 'fluidsynth': [wrapper script and desktop file](https://github.com/captnfab/PianoBooster/tree/master/tools/fluidsynth).

## How do I hear the sound on Arch Linux?

Timidity in Arch Linux does not work out of box.
To use Freepats (freepats-general-midi package) with TiMidity, add the following lines to /etc/timidity++/timidity.cfg:

`soundfont /usr/share/soundfonts/freepats-general-midi.sf2`

[Arch Linux Wiki](https://wiki.archlinux.org/index.php/Timidity)

Potentially needs to disable pulseaudio to use ALSA.

JACK can be used by jack or jack2. jack has better debug logs than jack2 to see errors, but they work equally.
But JACK does not work out of box, needs to configure it.

run:

`sudo sed -i "s|# End of file|@audio - rtprio 99|g" /etc/security/limits.conf`

run:

`sudo usermod -a -G audio exton`, where exton is user

log out and log in

stop pulseaudio:

`systemctl --user stop pulseaudio.socket`

run script:

`pianobooster-fluidsynth`

start pulseaudio after:

`systemctl --user start pulseaudio.socket`

*Note:* Fluidsynth works without JACK with ALSA out of box even without conflicts with pulseaudio.

## How do I hear the sound on Mac?

While there are many possible choices General Midi sound generators for the Mac
(Timidity, FluidSynth or an external MIDI Synth), the easiest way is to use
SimpleSynth, which can be obtained for free (MIT license) frome [here](http://notahat.com/simplesynth).

This must be started before Pianobooster, and also allows to load custom Soundfonts.

## How to use Fluidsynth with JACK

Just enable `use_jack="1"` in /usr/bin/pianobooster-fluidsynth. If `use_jack="0"`, then JACK is disabled.

## Why is there a delay between pressing the note and hearing the sound?

The delay between pressing the note and hearing the sound is called the latency.
Unfortunately some sound generators introduce a latency delay between receiving
the command to play a note and producing the audible sound. This unwanted delay
can make it extremely difficult to play along with the music.

The MS GM Wavetable Synthesizer that comes with Windows also suffers from this
problem as it introduces a noticeable delay of 150 milliseconds (0.15 seconds).

## My sound generator has high latency, what can I do?

The first thing to do is to turn up the speakers on your piano keyboard
(provided that you are using a piano keyboard that has it's own sound generator)
so that you can hear what you are playing without a delay.
Then aim to use the **'Play Along'** mode as much as possible.

To use the **'Follow You'** mode you will need set the latency value (see the next question)
or alternately use a realtime sound generator with low latency (recommended, For Linux, BSD Unix see
[fluidsynth Low Latency page](https://github.com/FluidSynth/fluidsynth/wiki/LowLatency).

## What is the latency fix?

The Latency fix aims to counteract the latency delay of the sound generator by
deliberately running the musical accompaniment several milliseconds ahead of
what you are playing on the piano to try and cancel out this delay within the
sound generator.

The latency value is entered from the 'settings/Midi setup' page.
If you  are using the MS GM Wavetable Synthesizer then try entering a value for the delay of 150 mSec.

Whenever you set a value for the latency then the  instrument sound
for your piano is automatically set to 'None' to mute
the delayed sound of your playing through the sound generator.
Remember to turn up the volume of the speakers on your piano.
If your piano does not have it's own speakers or you are still
using the PC keyboard then this fix will not work, sorry.

Unfortunately when you stop playing using the **'Follow You'** mode
then the music will stop in the wrong place (because the music
was deliberately being run ahead of your playing).
The use of the latency fix is less than ideal and so the **best**
solution is to change to different realtime sound generator that has a low latency delay.

Optional workaround: Try using a real time kernel.

## What value should I use for the latency fix?

To work out the value of the delay first enable the timing markers from the preferences menu.
(These are the white crosses that are drawn over the notes that show *when* you pressed the note).
Then using the **'Play Along'** mode look at the timing markers to see
if you are playing ahead or behind the notes.
If you are playing consistently behind the beat then try increasing
the latency value by say 100 milliseconds.
Conversely if the timing markers show you are playing ahead of the beat
then try decreasing the latency slightly.

It is always best to use a value that shows you are
playing *slightly ahead of the beat* rather than behind the beat.
(This is because Piano Booster in **'Follow You'** mode will slow down
the music if you are playing behind the beat.)

To disable the latency fix enter a value of zero.
