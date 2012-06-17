#CONFIG += USE_FLUIDSYNTH
CONFIG += release
#CONFIG += console

TRANSLATIONS = ../translations/pianobooster_ja.ts \
               ../translations/pianobooster_es.ts \
               ../translations/pianobooster_blank.ts \


USE_FLUIDSYNTH {
# Note The FLUIDSYNTH_INPLACE_DIR dir is used mainly used when compiling on windows
# You normally do not need to set it
#FLUIDSYNTH_INPLACE_DIR = ../../fluidsynth-1.0.9
message(building using fluidsynth)
DEFINES += PB_USE_FLUIDSYNTH
}

CONFIG += precompile_header
PRECOMPILED_HEADER = precompile/precompile.h

HEADERS   = precompile/precompile.h \
            QtWindow.h \
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

INCLUDEPATH += rtmidi

OBJECTS_DIR = tmp

win32 {
  DEFINES += __WINDOWS_MM__ _WIN32
  LIBS += libwinmm
}

unix {
  DEFINES += __LINUX_ALSASEQ__
  LIBS += -lasound
}

USE_FLUIDSYNTH {

     SOURCES   += MidiDeviceFluidSynth.cpp

    !isEmpty(FLUIDSYNTH_INPLACE_DIR) {

    !exists( $${FLUIDSYNTH_INPLACE_DIR}/include/fluidsynth.h ) {
       error( "No $${FLUIDSYNTH_INPLACE_DIR}/include/fluidsynth.h file found" )
    }
    message(fluidsynth FLUIDSYNTH_INPLACE_DIR = $${FLUIDSYNTH_INPLACE_DIR})
      INCLUDEPATH += $${FLUIDSYNTH_INPLACE_DIR}/include/
      win32:LIBS += $${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.dll.a
      unix:LIBS += $${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.a

    }
}

QT += xml opengl

# enable the console window
#QT+=testlib





unix {

   isEmpty( PREFIX ) { PREFIX = /usr/local }

   target.path = $$PREFIX/bin

   desktop.path = $$PREFIX/share/applications
   desktop.files = pianobooster.desktop

   pixmaps.path = $$PREFIX/share/pixmaps
   pixmaps.files = images/pianobooster.png

   docs.path = $$PREFIX/share/doc/pianobooster
   docs.files = ../README.txt

   INSTALLS += target desktop pixmaps docs
}
