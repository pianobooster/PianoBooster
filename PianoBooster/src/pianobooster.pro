HEADERS   = QtWindow.h \
            GlView.h \
            GuiTopBar.h \
            GuiSidePanel.h \
            GuiMidiSetupDialog.h \
            GuiKeyboardSetupDialog.h \
            GuiPreferencesDialog.h \
            GuiSongDetailsDialog.h \
            GuiLoopingPopup.h

FORMS    =  GuiTopBar.ui \
            GuiSidePanel.ui \
            GuiMidiSetupDialog.ui \
            GuiKeyboardSetupDialog.ui \
            GuiPreferencesDialog.ui \
            GuiSongDetailsDialog.ui \
            GuiLoopingPopup.ui

RESOURCES  = application.qrc

SOURCES   = QtMain.cpp  \
            QtWindow.cpp \
            GuiTopBar.cpp \
            GuiSidePanel.cpp \
            GuiMidiSetupDialog.cpp \
            GuiKeyboardSetupDialog.cpp \
            GuiPreferencesDialog.cpp \
            GuiSongDetailsDialog.cpp \
            GuiLoopingPopup.cpp \
            GlView.cpp \
            MidiFile.cpp  \
            MidiTrack.cpp  \
            Song.cpp  \
            Conductor.cpp  \
            Util.cpp \
            Chord.cpp  \
            Tempo.cpp \
            MidiDevice.cpp \
            MidiDeviceRt.cpp \
            MidiDeviceFluidSynth.cpp \
            rtmidi/RtMidi.cpp \
            StavePosition.cpp \
            Score.cpp \
            Cfg.cpp \
            Piano.cpp \
            Draw.cpp \
            Scroll.cpp \
            Notation.cpp \
            TrackList.cpp \
            Rating.cpp \
            Bar.cpp \
            Settings.cpp \
            Merge.cpp \

RC_FILE     = pianobooster.rc

#INCLUDEPATH = C:/download/misc/ljb/fluidsynth-1.0.9/include/ rtmidi
INCLUDEPATH = /home/louis/build/fluidsynth-1.0.9/include/ rtmidi

#DEFINES += __WINDOWS_MM__ _WIN32
DEFINES += __LINUX_ALSASEQ__


#LIBS += C:/download/misc/ljb/fluidsynth-1.0.9/src/.libs/libfluidsynth.dll.a libwinmm
LIBS += -lasound /home/louis/build/fluidsynth-1.0.9/src/.libs/libfluidsynth.a 

#libwinmm


QT           += xml opengl

# install
target.path = pianobooster
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro images
sources.path = pianobooster
INSTALLS += target sources

