# Frequently Asked Questions

## Where are the music files?

Piano Booster uses MIDI music files (.mid) or karaoke files (.kar) to play music.

See [this thread](http://piano-booster.2625608.n2.nabble.com/Pieces-of-music-that-I-have-enjoyed-playing-please-add-to-this-list-td1600345.html)
on the forum which list useful source of midi files on the net.
Please add to this list if you find any pieces of music that you enjoy playing.

## How do I hear the sound on Linux (Ubuntu)?

To hear the sound Piano Booster needs a MIDI sound generator,
there are two different General Midi sound generators available on Linux, 'fluidsynth' and 'timidity'.
To install fluidsynth you must install the following packages 'fluidsynth' 'fluid-soundfont-gm' and
'fluid-soundfont-gs' using your favourite package manager.
To start the fluidsynth sound generator copy and paste the following command line (one very long command line).

```bash
fluidsynth -C 0 -R 0 -r 22050 -c 6 -z 128 -l -a alsa -o audio.alsa.device=plughw:0 -o midi.alsa_seq.id=fs /usr/share/sounds/sf2/FluidR3_GM.sf2 /usr/share/sounds/sf2/FluidR3_GS.sf2
```
If you have a fast machine then you can leave out the flags  `-C 0 -R 0 -r 22050` which turns off the reverb, chorus and reduce the sample rate.

The flags `-c 6 -z 128` control the latency try `-c 5 -z 128` for less latency
but at the risk of audio drop outs.

The flags `-o audio.alsa.device=plughw:0` bypasses the Ubuntu Pulse Audio layer
which caused a lot of latency delay but unfortunately these flags may also
bypass the desktop volume controls.

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

## How do I hear the sound on Mac?

While there are many possible choices General Midi sound generators for the Mac
(Timidity, FluidSynth or an external MIDI Synth), the easiest way is to use
SimplSynth, which can be obtained for free (MIT license) frome [here](http://notahat.com/simplesynth)

This must be started before Pianobooster, and also allows to load custom Soundfonts.

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
or alternately use a realtime sound generator with low latency (recommended, For Linux see
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
