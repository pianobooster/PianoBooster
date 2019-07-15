#CONFIG += USE_FLUIDSYNTH
CONFIG += release
#CONFIG += console

CONFIG += link_pkgconfig

# default
isEmpty(USE_FTGL): USE_FTGL="ON"
isEmpty(USE_TIMIDITY): USE_TIMIDITY="OFF"
isEmpty(NO_DOCS): NO_DOCS="OFF"
isEmpty(NO_LICENSE): NO_LICENSE="OFF"
isEmpty(WITH_MAN): WITH_MAN="OFF"
isEmpty(WITH_TIMIDITY): WITH_TIMIDITY="OFF"
isEmpty(WITH_FLUIDSYNTH): WITH_FLUIDSYNTH="OFF"
isEmpty(NO_LANGS): NO_LANGS="OFF"

# install all languages always
INSTALL_ALL_LANGS="ON"

TRANSLATIONS = $$files(translations/*.ts)

CONFIG(debug, debug|release): DEFINES += IS_DEBUG

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

contains(USE_SYSTEM_RTMIDI, ON){
    message(building using system rtmidi)
    DEFINES+=USE_SYSTEM_RTMIDI
    PKGCONFIG += rtmidi
}else{
    message(building using bundled rtmidi)
    INCLUDEPATH += src/3rdparty
    SOURCES+= src/3rdparty/rtmidi/RtMidi.cpp
}

contains(USE_TIMIDITY, ON){
    DEFINES += PB_USE_TIMIDITY
}
contains(USE_FTGL, ON){
    message(building using ftgl)
    PKGCONFIG += ftgl
}else{
    message(building without ftgl)
    DEFINES += NO_USE_FTGL
}

contains(NO_LANGS, ON){
    message(building without languages)
    DEFINES += NO_LANGS
}

isEmpty(DATA_DIR){
    DATA_DIR=share/games/pianobooster
}
message("DATA_DIR:" $$DATA_DIR)
DEFINES += DATA_DIR="\\\""$$DATA_DIR"\\\""

RC_FILE = src/pianobooster.rc

OBJECTS_DIR = tmp

win32 {
  DEFINES += __WINDOWS_MM__ _WIN32
  LIBS += libwinmm
}

unix {
  DEFINES += __LINUX_ALSASEQ__
  LIBS += -lpthread -lGL
}


contains (USE_FLUIDSYNTH, ON) {
    message("building using fluidsynth")
    DEFINES += PB_USE_FLUIDSYNTH

    SOURCES   += src/MidiDeviceFluidSynth.cpp
    !isEmpty(FLUIDSYNTH_INPLACE_DIR) {
	!exists( $${FLUIDSYNTH_INPLACE_DIR}/include/fluidsynth.h ) {
    	    error( "No $${FLUIDSYNTH_INPLACE_DIR}/include/fluidsynth.h file found" )
	}
	message(fluidsynth FLUIDSYNTH_INPLACE_DIR = $${FLUIDSYNTH_INPLACE_DIR})
        INCLUDEPATH += $${FLUIDSYNTH_INPLACE_DIR}/include/
        win32:LIBS += $${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.dll.a
        unix:LIBS += $${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.a
    }else{
	PKGCONFIG += fluidsynth
    }
}

QT += xml opengl widgets

# enable the console window
#QT+=testlib

isEmpty(QMAKE_LRELEASE) {
    win32|os2:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    unix {
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease-qt5 }
    } else {
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
    }
}

!win32 {
  system($${QMAKE_LRELEASE} -silent $${_PRO_FILE_} 2> /dev/null)
}
win32 {
  system($$[QT_INSTALL_BINS]\\lrelease.exe $${_PRO_FILE_})
}

isEmpty( PREFIX ) { PREFIX = /usr }
message("PREFIX:" $$PREFIX)
DEFINES += PREFIX="\\\""$$PREFIX"\\\""

unix {
   target.path = $$PREFIX/bin

   contains(NO_DOCS, OFF){
      message(building with docs)
      docs.path = $$PREFIX/share/doc/pianobooster
      docs.files = README.md ReleaseNotes.txt doc/faq.md
      INSTALLS += docs
   }

   contains(NO_LICENSE, OFF){
      message(building with license)
      license.path = $$PREFIX/share/licenses/pianobooster
      license.files = license.txt
      INSTALLS += license
   }

   contains(WITH_MAN, ON){
      message(building with man)
      man.path = $$PREFIX/share/man/man6/
      man.files = pianobooster.6
      INSTALLS += man
   }

   contains(WITH_TIMIDITY, ON){
      message(building with timidity)

      timidity.path = $$PREFIX/bin
      timidity.files = tools/timidity/pianobooster-timidity
      INSTALLS += timidity

      timidity_desktop.path = $$PREFIX/share/applications
      timidity_desktop.files = tools/timidity/pianobooster-timidity.desktop
      INSTALLS += timidity_desktop
   }

   contains(WITH_FLUIDSYNTH, ON){
      message(building with fluidsynth)
      fluidsynth.path = $$PREFIX/bin
      fluidsynth.files = tools/fluidsynth/pianobooster-fluidsynth
      INSTALLS += fluidsynth

      fluidsynth_desktop.path = $$PREFIX/share/applications
      fluidsynth_desktop.files = tools/fluidsynth/pianobooster-fluidsynth.desktop
      INSTALLS += fluidsynth_desktop
   }

   !contains(USE_SYSTEM_FONT, ON){
      message(building using system font)
      font.path = $$PREFIX/$$DATA_DIR/fonts
      font.files = src/fonts/DejaVuSans.ttf
      INSTALLS += font
   }

   !isEmpty(USE_FONT){
      message(building with specified font)
      myfont.path = $$PREFIX/$$DATA_DIR/fonts
      myfont.files = $$USE_FONT
      INSTALLS += myfont
      DEFINES += USE_FONT=$$USE_FONT
   }

   contains(NO_LANGS, OFF){
	   updateqm.input = TRANSLATIONS
	   updateqm.output = translations/${QMAKE_FILE_BASE}.qm
	   updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm translations/${QMAKE_FILE_BASE}.qm
	   updateqm.CONFIG += no_link target_predeps
	   QMAKE_EXTRA_COMPILERS += updateqm
	
	   data_langs.path = $$PREFIX/$$DATA_DIR/translations
	   data_langs.files = translations/*.qm translations/langs.json
	   INSTALLS += data_langs
	
	   data_langs_fix.path = $$PREFIX/$$DATA_DIR/translations/
	   data_langs_fix.extra = rm ${INSTALL_ROOT}$$PREFIX/$$DATA_DIR/translations/music_blank.qm \
               ${INSTALL_ROOT}$$PREFIX/$$DATA_DIR/translations/pianobooster_blank.qm
	   INSTALLS += data_langs_fix
   }

   desktop.path = $$PREFIX/share/applications
   desktop.files = pianobooster.desktop

   icon32.path = $$PREFIX/share/icons/hicolor/32x32/apps
   icon32.files = icons/hicolor/32x32/pianobooster.png

   icon48.path = $$PREFIX/share/icons/hicolor/48x48/apps
   icon48.files = icons/hicolor/48x48/pianobooster.png

   icon64.path = $$PREFIX/share/icons/hicolor/64x64/apps
   icon64.files = icons/hicolor/64x64/pianobooster.png

   music.path = $$PREFIX/$$DATA_DIR/music
   music.files = music/BoosterMusicBooks.zip

   INSTALLS += target desktop icon32 icon48 icon64 music
}
