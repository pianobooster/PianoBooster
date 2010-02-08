.. include:: includestart.rst

.. raw:: html

  <h1>Frequently Asked Questions</h1>

.. contents:: *Contents*

Where are the music files?
-----------------------------

Piano Booster uses MIDI music files (.mid) or karaoke files (.kar) to play music
but none of these files are included with the program so you will have to get them from the NET.
In particular there  are some high quality demo midi files contained in three zip files
available from `this page <http://www.pianobooster.pwp.blueyonder.co.uk>`_.
These are demos of commercially produced MIDI files that used to be supplied with music tutor books but now these books are sold without any MIDI files.

Also see `this thread <http://n2.nabble.com/Pieces-of-music-that-I-have-enjoyed-playing---please-add-to-this-list-td1600345.html>`_
on the forum which list useful source of midi files on the net.
Please add to this list if you find any pieces of music that you enjoy playing.


How do I hear the sound on Linux (Ubuntu)?
-------------------------------------------

To hear the sound Piano Booster needs a MIDI sound generator,
there are two different General Midi sound generators available on Linux, 'fluidsynth' and 'timidty'.
To install fluidsynth you must install the following packages 'fluidsynth' 'fluid-soundfont-gm' and
'fluid-soundfont-gs' using your favourite package manager.
To start the fluidsynth sound generator copy and paste the following command line (one very long command line).

::

 fluidsynth -C 0 -R 0 -r 22050 -c 6 -z 128 -l -a alsa -o audio.alsa.device=plughw:0 -o midi.alsa_seq.id=fs /usr/share/sounds/sf2/FluidR3_GM.sf2 /usr/share/sounds/sf2/FluidR3_GS.sf2


If you have a fast machine then you can leave out the flags  '-C 0 -R 0 -r 22050' which turns off
the reverb, chorus and reduce the sample rate.
The flags '-c 6 -z 128' control the latency try '-c 5 -z 128' for less latency
but at the risk of audio drop outs.
The flags '-o audio.alsa.device=plughw:0'
bypasses the Ubuntu Pulse Audio layer which caused a lot of latency delay but unfortunately these flags may also bypass
the desktop volume controls.

For more information on running fluidsynth with low latency see this page.

  http://fluidsynth.resonance.org/trac/wiki/LowLatency

On PianoBooster set the Setup/Midi Setup/Midi Output Device: to "FLUID Synth (fs):0"

If you get problems with unsteady playback or the audio cutting out then try closing all other programs,
turning off your wireless network and unplug any network cable.

How do I hear the sound on Mac?
---------------------------------

While there are many possible choices General Midi sound generators
for the Mac (Timidity, FluidSynth or an external MIDI
Synth), the easiest way is to use SimplSynth, which can be obtained for free (MIT license)
from:

    http://notahat.com/simplesynth

This must be started before Pianobooster, and also allows to load custom Soundfonts.

Why is there a delay between pressing the note and hearing the sound?
---------------------------------------------------------------------

The delay between pressing the note and hearing the sound is called the latency.
Unfortunately some sound generators introduce a latency delay between receiving
the command to play a note and producing the audible sound.
This unwanted delay can make it extremely difficult to play along with the music.

The MS GM Wavetable Synthesizer that comes with Windows also suffers from this problem
as it introduces a noticeable delay of 150 milliseconds (0.15 seconds).


My sound generator has high latency, what can I do?
--------------------------------------------------------

The first thing to do is to turn up the speakers on your piano keyboard
(provided that you are using a piano keyboard that has it's own sound generator)
so that you can hear what you are playing without a delay.
Then aim to use the **'Play Along'** mode as much as possible.

To use the **'Follow You'** mode you will need set the latency value (see the next question)
or alternately use a realtime sound generator with low latency (recommended, For Linux see `fluidsynth Low Latency page <http://fluidsynth.resonance.org/trac/wiki/LowLatency>`_).


What is the latency fix?
----------------------------

The Latency fix aims to counteract the latency delay of the sound generator by deliberately
running the musical accompaniment several milliseconds ahead of what you are playing on the
piano to try and cancel out this delay within the sound generator.

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


What value should I use for the latency fix?
-----------------------------------------------------

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



.. How do I get the very latest code from subversion
  Ensure you have the subversion client 'svn' installed.
  Then run the following command on the command line.
  Please be aware that this is development code::
  svn co https://pianobooster.svn.sourceforge.net/svnroot/pianobooster/trunk/PianoBooster



.. include:: includeend.rst

