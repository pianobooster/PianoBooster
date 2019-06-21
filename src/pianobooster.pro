#CONFIG += USE_FLUIDSYNTH
CONFIG += release
#CONFIG += console

TRANSLATIONS = ../translations/pianobooster_af.ts \
               ../translations/pianobooster_am.ts \
               ../translations/pianobooster_ar.ts \
               ../translations/pianobooster_as.ts \
               ../translations/pianobooster_ast.ts \
               ../translations/pianobooster_az.ts \
               ../translations/pianobooster_be.ts \
               ../translations/pianobooster_bg.ts \
               ../translations/pianobooster_blank.ts \
               ../translations/pianobooster_bn.ts \
               ../translations/pianobooster_br.ts \
               ../translations/pianobooster_bs_BA.ts \
               ../translations/pianobooster_bs.ts \
               ../translations/pianobooster_ca.ts \
               ../translations/pianobooster_crh.ts \
               ../translations/pianobooster_csb.ts \
               ../translations/pianobooster_cs.ts \
               ../translations/pianobooster_cy.ts \
               ../translations/pianobooster_da.ts \
               ../translations/pianobooster_de.ts \
               ../translations/pianobooster_dz.ts \
               ../translations/pianobooster_el.ts \
               ../translations/pianobooster_eo.ts \
               ../translations/pianobooster_es.ts \
               ../translations/pianobooster_et.ts \
               ../translations/pianobooster_eu.ts \
               ../translations/pianobooster_fa.ts \
               ../translations/pianobooster_fi.ts \
               ../translations/pianobooster_fo.ts \
               ../translations/pianobooster_frp.ts \
               ../translations/pianobooster_fr.ts \
               ../translations/pianobooster_fur.ts \
               ../translations/pianobooster_fy.ts \
               ../translations/pianobooster_ga.ts \
               ../translations/pianobooster_gd.ts \
               ../translations/pianobooster_gl.ts \
               ../translations/pianobooster_gu.ts \
               ../translations/pianobooster_he.ts \
               ../translations/pianobooster_hi.ts \
               ../translations/pianobooster_hr.ts \
               ../translations/pianobooster_hu.ts \
               ../translations/pianobooster_hy.ts \
               ../translations/pianobooster_id.ts \
               ../translations/pianobooster_is.ts \
               ../translations/pianobooster_it.ts \
               ../translations/pianobooster_ja.ts \
               ../translations/pianobooster_ka.ts \
               ../translations/pianobooster_kk.ts \
               ../translations/pianobooster_kn.ts \
               ../translations/pianobooster_ko.ts \
               ../translations/pianobooster_ks.ts \
               ../translations/pianobooster_ku.ts \
               ../translations/pianobooster_lt.ts \
               ../translations/pianobooster_lv.ts \
               ../translations/pianobooster_mai.ts \
               ../translations/pianobooster_mg.ts \
               ../translations/pianobooster_mi.ts \
               ../translations/pianobooster_ml.ts \
               ../translations/pianobooster_mn.ts \
               ../translations/pianobooster_mr.ts \
               ../translations/pianobooster_ms.ts \
               ../translations/pianobooster_my.ts \
               ../translations/pianobooster_nb.ts \
               ../translations/pianobooster_nds.ts \
               ../translations/pianobooster_ne.ts \
               ../translations/pianobooster_nl.ts \
               ../translations/pianobooster_nn.ts \
               ../translations/pianobooster_oc.ts \
               ../translations/pianobooster_or.ts \
               ../translations/pianobooster_pa.ts \
               ../translations/pianobooster_pl.ts \
               ../translations/pianobooster_ps.ts \
               ../translations/pianobooster_pt_BR.ts \
               ../translations/pianobooster_pt.ts \
               ../translations/pianobooster_ro.ts \
               ../translations/pianobooster_ru.ts \
               ../translations/pianobooster_se.ts \
               ../translations/pianobooster_si.ts \
               ../translations/pianobooster_sk.ts \
               ../translations/pianobooster_sl.ts \
               ../translations/pianobooster_sq.ts \
               ../translations/pianobooster_sr@latin.ts \
               ../translations/pianobooster_sr.ts \
               ../translations/pianobooster_sv.ts \
               ../translations/pianobooster_ta.ts \
               ../translations/pianobooster_te.ts \
               ../translations/pianobooster_tg.ts \
               ../translations/pianobooster_th.ts \
               ../translations/pianobooster_tr.ts \
               ../translations/pianobooster_tt.ts \
               ../translations/pianobooster_uk.ts \
               ../translations/pianobooster_uz.ts \
               ../translations/pianobooster_vi.ts \
               ../translations/pianobooster_wa.ts \
               ../translations/pianobooster_xh.ts \
               ../translations/pianobooster_zh_HK.ts \
               ../translations/pianobooster_zh.ts \

USE_FLUIDSYNTH {
# Note The FLUIDSYNTH_INPLACE_DIR dir is used mainly used when compiling on windows
# You normally do not need to set it
#FLUIDSYNTH_INPLACE_DIR = ../../fluidsynth-1.0.9
message(building using fluidsynth)
DEFINES += PB_USE_FLUIDSYNTH
}

CONFIG(debug, debug|release): DEFINES += IS_DEBUG

HEADERS   = QtWindow.h \
            GlView.h \
            GuiTopBar.h \
            GuiSidePanel.h \
            GuiMidiSetupDialog.h \
            GuiKeyboardSetupDialog.h \
            GuiPreferencesDialog.h \
            GuiSongDetailsDialog.h \
            GuiLoopingPopup.h \
            Settings.h \
            Draw.h \
            TrackList.h

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
            3rdparty/rtmidi/RtMidi.cpp \
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

INCLUDEPATH += 3rdparty

OBJECTS_DIR = tmp

CONFIG += link_pkgconfig
PKGCONFIG += ftgl

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

QT += xml opengl widgets

# enable the console window
#QT+=testlib





unix {

   isEmpty( PREFIX ) { PREFIX = /usr/local }

   target.path = $$PREFIX/bin

   desktop.path = $$PREFIX/share/applications
   desktop.files = ../pianobooster.desktop

   icon32.path = $$PREFIX/icons/hicolor/32x32/apps
   icon32.files = ../icons/hicolor/32x32/pianobooster.png

   icon48.path = $$PREFIX/icons/hicolor/48x48/apps
   icon48.files = ../icons/hicolor/48x48/pianobooster.png

   icon64.path = $$PREFIX/icons/hicolor/64x64/apps
   icon64.files = ../icons/hicolor/64x64/pianobooster.png

   docs.path = $$PREFIX/share/doc/pianobooster
   docs.files = ../README.txt

   INSTALLS += target desktop icon32 icon48 icon64 docs
}
