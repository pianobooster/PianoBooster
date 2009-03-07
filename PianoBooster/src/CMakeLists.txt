#
cmake_minimum_required(VERSION 2.4)

IF(MSVC)
    MESSAGE("GUI system is MSVC")
ENDIF(MSVC)
IF(WIN32)
    MESSAGE("GUI system is WIN32 ${CMAKE_GENERATOR}")
    SET(CMAKE_COLOR_MAKEFILE OFF)
ENDIF(WIN32)

# set project's name
PROJECT( pianobooster )

SET(PB_BASE_SRCS MidiFile.cpp MidiDevice.cpp MidiTrack.cpp Song.cpp Conductor.cpp Util.cpp
    Chord.cpp Tempo.cpp rtmidi/RtMidi.cpp)
SET(PB_BASE_HDR MidiFile.h MidiDevice.h MidiTrack.h Song.h Conductor.h Rating.h Util.h
    MidiDevice.h Chord.h Tempo.h rtmidi/RtMidi.h)

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
)

# some .ui files
SET( PIANOBOOSTER_UIS
    ./GuiTopBar.ui
    ./GuiSidePanel.ui
    ./GuiMidiSetupDialog.ui
    ./GuiKeyboardSetupDialog.ui
    ./GuiPreferencesDialog.ui
)

# and finally an resource file
SET( PIANOBOOSTER_RCS
    ./application.qrc
)

# enable warnings
ADD_DEFINITIONS(-Wall)

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
SET(QT_USE_QTOPENGL TRUE)


FIND_PACKAGE( OpenGL REQUIRED )
MESSAGE("the GL include directory ${OPENGL_INCLUDE_DIR}")


# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
FIND_PACKAGE( Qt4 REQUIRED )

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )

# this command will generate rules that will run rcc on all files from PIANOBOOSTER_RCS
# in result PIANOBOOSTER_RC_SRCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( PIANOBOOSTER_RC_SRCS ${PIANOBOOSTER_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( PIANOBOOSTER_UI_HDRS ${PIANOBOOSTER_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( PIANOBOOSTER_MOC_SRCS ${PIANOBOOSTER_MOC_HDRS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${OPENGL_INCLUDE_DIR})

# tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
LINK_LIBRARIES( ${QT_LIBRARIES} )

# Add in the link libraries for each operating system
IF(${CMAKE_SYSTEM} MATCHES "Linux")
    ADD_DEFINITIONS(-D__LINUX_ALSASEQ__)
    LINK_LIBRARIES (asound)
ENDIF(${CMAKE_SYSTEM} MATCHES "Linux")

IF(${CMAKE_SYSTEM} MATCHES "Windows")
    FIND_PACKAGE(WINDRES REQUIRED)
    ADD_DEFINITIONS(-D__WINDOWS_MM__ -D_WIN32)
    LINK_LIBRARIES(winmm opengl32)
ENDIF(${CMAKE_SYSTEM} MATCHES "Windows")

IF(${CMAKE_SYSTEM} MATCHES "Darwin")
    ADD_DEFINITIONS(-D__MACOSX_CORE__)
    LINK_LIBRARIES("-framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework OpenGL")
ENDIF(${CMAKE_SYSTEM} MATCHES "Darwin")


# here we instruct CMake to build "pianobooster" executable from all of the source files
ADD_EXECUTABLE( pianobooster ${PIANOBOOSTER_SRCS} ${PIANOBOOSTER_MOC_SRCS}
    ${PIANOBOOSTER_RC_SRCS}
    ${PIANOBOOSTER_UI_HDRS}
)

SET_TARGET_PROPERTIES(pianobooster PROPERTIES LINK_FLAGS "-mwindows")


