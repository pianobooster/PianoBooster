# Cmake File for Piano Booster
# for the debug build type cmake -DCMAKE_BUILD_TYPE=Debug
SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(USE_FLUIDSYNTH OFF)

# The inplace directory is mainly for windows builds
# SET(FLUIDSYNTH_INPLACE_DIR C:/download/misc/ljb/fluidsynth-1.0.9)
SET(FLUIDSYNTH_INPLACE_DIR /home/louis/build/fluidsynth-1.0.9)


# Testing precompiled headers  it does not work -- leave as OFF.
SET(USE_PCH OFF)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

IF(WIN32)
    MESSAGE("GUI system is WIN32 ${CMAKE_GENERATOR}")
    SET(CMAKE_COLOR_MAKEFILE OFF)
ENDIF(WIN32)

# set project's name
PROJECT( pianobooster )

# enable warnings
ADD_DEFINITIONS(-Wall)

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)


FIND_PACKAGE( OpenGL REQUIRED )

# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
FIND_PACKAGE( Qt4 REQUIRED )

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )

IF (USE_PCH)
INCLUDE(precompile/PCHSupport_26.cmake)
INCLUDE_DIRECTORIES( precompile .)
ENDIF(USE_PCH)


# Add in the link libraries for each operating system
IF(${CMAKE_SYSTEM} MATCHES "Linux")
    #FIND_PACKAGE(PkgConfig REQUIRED)
    #PKG_CHECK_MODULES(ALSA REQUIRED alsa>=1.0)
    #IF(ALSA_FOUND)
        ADD_DEFINITIONS(-D__LINUX_ALSASEQ__)
        LINK_LIBRARIES (asound)
        LINK_LIBRARIES (pthread)
        LINK_LIBRARIES (GL)
    #ELSE(ALSA_FOUND)
    #    MESSAGE(FATAL_ERROR "Please install the 'libasound2-dev' package and then try again")
    #ENDIF(ALSA_FOUND)

ENDIF(${CMAKE_SYSTEM} MATCHES "Linux")

IF(${CMAKE_SYSTEM} MATCHES "Windows")
#    FIND_PACKAGE(WINDRES REQUIRED)
    ADD_DEFINITIONS(-D__WINDOWS_MM__ -D_WIN32)
    LINK_LIBRARIES(winmm opengl32)
ENDIF(${CMAKE_SYSTEM} MATCHES "Windows")

IF(${CMAKE_SYSTEM} MATCHES "Darwin")
    ADD_DEFINITIONS(-D__MACOSX_CORE__)
    LINK_LIBRARIES("-framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework OpenGL")
ENDIF(${CMAKE_SYSTEM} MATCHES "Darwin")

IF(USE_FLUIDSYNTH)
    ADD_DEFINITIONS(-DPB_USE_FLUIDSYNTH)
    MESSAGE("Building using fluidsynth")
    SET( PB_BASE_SRCS MidiDeviceFluidSynth.cpp )

    IF(FLUIDSYNTH_INPLACE_DIR)
        INCLUDE_DIRECTORIES(${FLUIDSYNTH_INPLACE_DIR}/include/)
        IF(WIN32)
            LINK_LIBRARIES( ${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.dll.a)
        ENDIF(WIN32)
        IF(UNIX)
            LINK_LIBRARIES(${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.so)
        ENDIF(UNIX)
    ELSEIF(FLUIDSYNTH_INPLACE_DIR)
        LINK_LIBRARIES( fluidsynth)
    ENDIF(FLUIDSYNTH_INPLACE_DIR)
ENDIF(USE_FLUIDSYNTH)



# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${OPENGL_INCLUDE_DIR} )

# tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
LINK_LIBRARIES( ${QT_LIBRARIES} )



SET(PB_BASE_SRCS MidiFile.cpp MidiTrack.cpp Song.cpp Conductor.cpp Util.cpp
    Chord.cpp Tempo.cpp MidiDevice.cpp MidiDeviceRt.cpp rtmidi/RtMidi.cpp ${PB_BASE_SRCS})
SET(PB_BASE_HDR MidiFile.h MidiTrack.h Song.h Conductor.h Rating.h Util.h
    Chord.h Tempo.h MidiDevice.h rtmidi/RtMidi.h)

# with SET() command you can change variables or define new ones
# here we define PIANOBOOSTER_SRCS variable that contains a list of all .cpp files
# note that we don't need \ at the end of line
SET( PIANOBOOSTER_SRCS
    QtMain.cpp
    QtWindow.cpp
    GuiTopBar.cpp
    GuiSidePanel.cpp
    GuiMidiSetupDialog.cpp
    GuiKeyboardSetupDialog.cpp
    GuiPreferencesDialog.cpp
    GuiSongDetailsDialog.cpp
    GuiLoopingPopup.cpp
    GlView.cpp
    ${PB_BASE_SRCS}
    StavePosition.cpp
    Score.cpp
    Cfg.cpp
    Piano.cpp
    Draw.cpp
    Scroll.cpp
    Notation.cpp
    TrackList.cpp
    Rating.cpp
    Bar.cpp
    Settings.cpp
    Merge.cpp
    #Band.cpp
    pianobooster.rc
    pianobooster.ico
#    precompile.h
)



# another list, this time it includes all header files that should be treated with moc
SET( PIANOBOOSTER_MOC_HDRS
    QtWindow.h
    GlView.h
    GuiTopBar.h
    GuiSidePanel.h
    GuiMidiSetupDialog.h
    GuiKeyboardSetupDialog.h
    GuiPreferencesDialog.h
    GuiSongDetailsDialog.h
    GuiLoopingPopup.h
)

# some .ui files
SET( PIANOBOOSTER_UIS
    ./GuiTopBar.ui
    ./GuiSidePanel.ui
    ./GuiMidiSetupDialog.ui
    ./GuiKeyboardSetupDialog.ui
    ./GuiPreferencesDialog.ui
    ./GuiSongDetailsDialog.ui
    ./GuiLoopingPopup.ui
)

# and finally an resource file
SET( PIANOBOOSTER_RCS
    ./application.qrc
)

# this command will generate rules that will run rcc on all files from PIANOBOOSTER_RCS
# in result PIANOBOOSTER_RC_SRCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( PIANOBOOSTER_RC_SRCS ${PIANOBOOSTER_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( PIANOBOOSTER_UI_HDRS ${PIANOBOOSTER_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( PIANOBOOSTER_MOC_SRCS ${PIANOBOOSTER_MOC_HDRS} )


# here we instruct CMake to build "pianobooster" executable from all of the source files
IF(APPLE)
    ADD_EXECUTABLE( pianobooster MACOSX_BUNDLE ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS})
ENDIF(APPLE)

IF(UNIX)
    ADD_EXECUTABLE( pianobooster pianobooster.rc ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} )
ENDIF(UNIX)

IF(WIN32)
    ADD_EXECUTABLE( pianobooster WIN32 pianobooster.rc ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} )
ENDIF(WIN32)

SET_TARGET_PROPERTIES(pianobooster PROPERTIES LINK_FLAGS "-mwindows")

IF (USE_PCH)
ADD_PRECOMPILED_HEADER( pianobooster ${CMAKE_CURRENT_SOURCE_DIR}/precompile/precompile.h )
ENDIF (USE_PCH)


INSTALL( FILES pianobooster.desktop DESTINATION share/applications )
INSTALL(TARGETS pianobooster RUNTIME DESTINATION bin)
#INSTALL( index.docbook INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en  SUBDIR kmidimon )

INSTALL( FILES ../README.txt  DESTINATION share/pianobooster/doc/ )

INSTALL ( FILES images/pianobooster.png DESTINATION share/pixmaps )

#INSTALL( FILES "${CMAKE_CURRENT_BINARY_DIR}/translations/*.qm  DESTINATION share/pianobooster/translations/ )



