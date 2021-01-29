CONFIG += release
#CONFIG += console

CONFIG += link_pkgconfig

# install all languages always

TRANSLATIONS = $$files(translations/*.ts)


HEADERS   = src/QtWindow.h \
            src/GlView.h \
            src/GuiTopBar.h \
            src/GuiSidePanel.h \
            src/GuiMidiSetupDialog.h \
            src/GuiKeyboardSetupDialog.h \
            src/GuiPreferencesDialog.h \
            src/GuiSongDetailsDialog.h \
            src/GuiLoopingPopup.h \
            src/Settings.h \
            src/Draw.h \
            src/TrackList.h

FORMS    =  src/GuiTopBar.ui \
            src/GuiSidePanel.ui \
            src/GuiMidiSetupDialog.ui \
            src/GuiKeyboardSetupDialog.ui \
            src/GuiPreferencesDialog.ui \
            src/GuiSongDetailsDialog.ui \
            src/GuiLoopingPopup.ui

RESOURCES  = src/application.qrc

SOURCES   = src/QtMain.cpp  \
            src/QtWindow.cpp \
            src/GuiTopBar.cpp \
            src/GuiSidePanel.cpp \
            src/GuiMidiSetupDialog.cpp \
            src/GuiKeyboardSetupDialog.cpp \
            src/GuiPreferencesDialog.cpp \
            src/GuiSongDetailsDialog.cpp \
            src/GuiLoopingPopup.cpp \
            src/GlView.cpp \
            src/MidiFile.cpp  \
            src/MidiTrack.cpp  \
            src/Song.cpp  \
            src/Conductor.cpp  \
            src/Util.cpp \
            src/Chord.cpp  \
            src/Tempo.cpp \
            src/MidiDevice.cpp \
            src/MidiDeviceRt.cpp \
            src/StavePosition.cpp \
            src/Score.cpp \
            src/Cfg.cpp \
            src/Piano.cpp \
            src/Draw.cpp \
            src/Scroll.cpp \
            src/Notation.cpp \
            src/TrackList.cpp \
            src/Rating.cpp \
            src/Bar.cpp \
            src/Settings.cpp \
            src/Merge.cpp



message(This pianobooster.pro file is only used when running lupdate.)
error(qmake is no longer supported by PianoBooster. Please use cmake instead.)

