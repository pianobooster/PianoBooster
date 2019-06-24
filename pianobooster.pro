#CONFIG += USE_FLUIDSYNTH
CONFIG += release
#CONFIG += console

CONFIG += link_pkgconfig

# default
isEmpty(USE_FTGL): USE_FTGL="ON"
isEmpty(NO_DOCS): NO_DOCS="OFF"
isEmpty(WITH_MAN): WITH_MAN="OFF"
isEmpty(WITH_TIMIDITY): WITH_TIMIDITY="OFF"
isEmpty(WITH_FLUIDSYNTH): WITH_FLUIDSYNTH="OFF"
isEmpty(INSTALL_ALL_LANGS): INSTALL_ALL_LANGS="OFF"

TRANSLATIONS = translations/pianobooster_af.ts \
               translations/pianobooster_am.ts \
               translations/pianobooster_ar.ts \
               translations/pianobooster_as.ts \
               translations/pianobooster_ast.ts \
               translations/pianobooster_az.ts \
               translations/pianobooster_be.ts \
               translations/pianobooster_bg.ts \
               translations/pianobooster_blank.ts \
               translations/pianobooster_bn.ts \
               translations/pianobooster_br.ts \
               translations/pianobooster_bs_BA.ts \
               translations/pianobooster_bs.ts \
               translations/pianobooster_ca.ts \
               translations/pianobooster_crh.ts \
               translations/pianobooster_csb.ts \
               translations/pianobooster_cs.ts \
               translations/pianobooster_cy.ts \
               translations/pianobooster_da.ts \
               translations/pianobooster_de.ts \
               translations/pianobooster_dz.ts \
               translations/pianobooster_el.ts \
               translations/pianobooster_eo.ts \
               translations/pianobooster_es.ts \
               translations/pianobooster_et.ts \
               translations/pianobooster_eu.ts \
               translations/pianobooster_fa.ts \
               translations/pianobooster_fi.ts \
               translations/pianobooster_fo.ts \
               translations/pianobooster_frp.ts \
               translations/pianobooster_fr.ts \
               translations/pianobooster_fur.ts \
               translations/pianobooster_fy.ts \
               translations/pianobooster_ga.ts \
               translations/pianobooster_gd.ts \
               translations/pianobooster_gl.ts \
               translations/pianobooster_gu.ts \
               translations/pianobooster_he.ts \
               translations/pianobooster_hi.ts \
               translations/pianobooster_hr.ts \
               translations/pianobooster_hu.ts \
               translations/pianobooster_hy.ts \
               translations/pianobooster_id.ts \
               translations/pianobooster_is.ts \
               translations/pianobooster_it.ts \
               translations/pianobooster_ja.ts \
               translations/pianobooster_ka.ts \
               translations/pianobooster_kk.ts \
               translations/pianobooster_kn.ts \
               translations/pianobooster_ko.ts \
               translations/pianobooster_ks.ts \
               translations/pianobooster_ku.ts \
               translations/pianobooster_lt.ts \
               translations/pianobooster_lv.ts \
               translations/pianobooster_mai.ts \
               translations/pianobooster_mg.ts \
               translations/pianobooster_mi.ts \
               translations/pianobooster_ml.ts \
               translations/pianobooster_mn.ts \
               translations/pianobooster_mr.ts \
               translations/pianobooster_ms.ts \
               translations/pianobooster_my.ts \
               translations/pianobooster_nb.ts \
               translations/pianobooster_nds.ts \
               translations/pianobooster_ne.ts \
               translations/pianobooster_nl.ts \
               translations/pianobooster_nn.ts \
               translations/pianobooster_oc.ts \
               translations/pianobooster_or.ts \
               translations/pianobooster_pa.ts \
               translations/pianobooster_pl.ts \
               translations/pianobooster_ps.ts \
               translations/pianobooster_pt_BR.ts \
               translations/pianobooster_pt.ts \
               translations/pianobooster_ro.ts \
               translations/pianobooster_ru.ts \
               translations/pianobooster_se.ts \
               translations/pianobooster_si.ts \
               translations/pianobooster_sk.ts \
               translations/pianobooster_sl.ts \
               translations/pianobooster_sq.ts \
               translations/pianobooster_sr@latin.ts \
               translations/pianobooster_sr.ts \
               translations/pianobooster_sv.ts \
               translations/pianobooster_ta.ts \
               translations/pianobooster_te.ts \
               translations/pianobooster_tg.ts \
               translations/pianobooster_th.ts \
               translations/pianobooster_tr.ts \
               translations/pianobooster_tt.ts \
               translations/pianobooster_uk.ts \
               translations/pianobooster_uz.ts \
               translations/pianobooster_vi.ts \
               translations/pianobooster_wa.ts \
               translations/pianobooster_xh.ts \
               translations/pianobooster_zh_HK.ts \
               translations/pianobooster_zh.ts \

contains(INSTALL_ALL_LANGS, ON){
  message(building with all languages)
  TRANSLATIONS = $$files(translations/*.ts)
}

TRANSLATIONS ~= s/(translations.music_blank.ts)//mi
TRANSLATIONS ~= s/(translations.pianobooster_blank.ts)//mi


USE_FLUIDSYNTH {
# Note The FLUIDSYNTH_INPLACE_DIR dir is used mainly used when compiling on windows
# You normally do not need to set it
#FLUIDSYNTH_INPLACE_DIR = ../../fluidsynth-1.0.9
message(building using fluidsynth)
DEFINES += PB_USE_FLUIDSYNTH
}

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
    PKGCONFIG += rtmidi
}else{
    message(building using bundled rtmidi)
    INCLUDEPATH += 3rdparty
    SOURCES+= src/3rdparty/rtmidi/RtMidi.cpp
}

contains(USE_FTGL, ON){
    message(building using ftgl)
    PKGCONFIG += ftgl
}else{
    message(building without ftgl)
    DEFINES += NO_USE_FTGL
}

RC_FILE = src/pianobooster.rc

OBJECTS_DIR = tmp

win32 {
  DEFINES += __WINDOWS_MM__ _WIN32
  LIBS += libwinmm
}

unix {
  DEFINES += __LINUX_ALSASEQ__
  LIBS += -lasound -lpthread -lGL
}

USE_FLUIDSYNTH {
    SOURCES   += src/MidiDeviceFluidSynth.cpp
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

unix {

   isEmpty( PREFIX ) { PREFIX = /usr }

   target.path = $$PREFIX/bin

   contains(NO_DOCS, OFF){
      message(building with docs)
      docs.path = $$PREFIX/share/doc/pianobooster
      docs.files = README.md ReleaseNotes.txt
      INSTALLS += docs
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
      font.path = $$PREFIX/share/games/pianobooster/fonts
      font.files = src/fonts/DejaVuSans.ttf
      INSTALLS += font
   }

   !isEmpty(USE_FONT){
      message(building with specified font)
      myfont.path = $$PREFIX/share/games/pianobooster/fonts
      myfont.files = $$USE_FONT
      INSTALLS += myfont
      DEFINES += USE_FONT=$$USE_FONT
   }

   updateqm.input = TRANSLATIONS
   updateqm.output = translations/${QMAKE_FILE_BASE}.qm
   updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm translations/${QMAKE_FILE_BASE}.qm
   updateqm.CONFIG += no_link target_predeps
   QMAKE_EXTRA_COMPILERS += updateqm

   data_langs.path = $$PREFIX/share/games/pianobooster/translations
   data_langs.files = translations/*.qm translations/langs.json
   INSTALLS += data_langs

   desktop.path = $$PREFIX/share/applications
   desktop.files = pianobooster.desktop

   icon32.path = $$PREFIX/share/icons/hicolor/32x32/apps
   icon32.files = icons/hicolor/32x32/pianobooster.png

   icon48.path = $$PREFIX/share/icons/hicolor/48x48/apps
   icon48.files = icons/hicolor/48x48/pianobooster.png

   icon64.path = $$PREFIX/share/icons/hicolor/64x64/apps
   icon64.files = icons/hicolor/64x64/pianobooster.png

   music.path = $$PREFIX/share/games/pianobooster/music
   music.files = music/BoosterMusicBooks.zip

   INSTALLS += target desktop icon32 icon48 icon64 music
}
