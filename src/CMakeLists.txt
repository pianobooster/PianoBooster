# Cmake File for Piano Booster

option(USE_FTGL "build with ftgl" ON)
option(USE_JACK "build with Jack (Only required of BSD Unix)" OFF)
option(USE_BUNDLED_RTMIDI "build with bundled rtmidi (for older distributions only)" OFF)
option(USE_SYSTEM_FONT "build with system font" OFF)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(WIN32)
    MESSAGE("GUI system is WIN32 ${CMAKE_GENERATOR}")
    SET(CMAKE_COLOR_MAKEFILE OFF)
endif(WIN32)

# set project's name
PROJECT( pianobooster )

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)

# enable warnings
add_compile_options(-Wall)

if(USE_FONT)
    add_compile_options("-DUSE_FONT=\"${USE_FONT}\"")
endif()

add_compile_options("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

if(NOT DATA_DIR)
    set(DATA_DIR "share/games/pianobooster")
endif()
add_compile_options("-DDATA_DIR=\"${DATA_DIR}\"")

if(NOT CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR "bin")
endif()

MESSAGE("PREFIX: " ${CMAKE_INSTALL_PREFIX})
MESSAGE("DATA_DIR: " ${DATA_DIR})
MESSAGE("CMAKE_INSTALL_BINDIR: " ${CMAKE_INSTALL_BINDIR})

FIND_PACKAGE( OpenGL REQUIRED )

include(FindPkgConfig)
FIND_PACKAGE( PkgConfig REQUIRED )

if(USE_FTGL)
   pkg_check_modules(FTGL ftgl)
   if(NOT FTGL_FOUND)
      MESSAGE(FATAL_ERROR "FTGL was not found")
   endif(NOT FTGL_FOUND)

   SET(FTGL_INCLUDE_DIR ${FTGL_INCLUDE_DIRS})
   SET(FTGL_LIBRARY ${FTGL_LIBRARIES})
else(USE_FTGL)
    add_compile_options("-DNO_USE_FTGL")
endif(USE_FTGL)

if(NO_LANGS)
    add_compile_options("-DNO_LANGS")
endif()

# Finds Qt5 libraries
FIND_PACKAGE( Qt5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools OpenGL Xml )

add_compile_options(-fPIC)

# Add in the link libraries for each operating system
if(${CMAKE_SYSTEM} MATCHES "Linux")
    ADD_DEFINITIONS(-D__LINUX_ALSA__)
    LINK_LIBRARIES (asound)
    LINK_LIBRARIES (pthread)
    LINK_LIBRARIES (GL)
endif(${CMAKE_SYSTEM} MATCHES "Linux")

if(${CMAKE_SYSTEM} MATCHES "Windows")
    ADD_DEFINITIONS(-D__WINDOWS_MM__ -D_WIN32)
    LINK_LIBRARIES(winmm opengl32)
endif(${CMAKE_SYSTEM} MATCHES "Windows")

if(${CMAKE_SYSTEM} MATCHES "Darwin")
    ADD_DEFINITIONS(-D__MACOSX_CORE__)
    LINK_LIBRARIES("-framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework OpenGL")
endif(${CMAKE_SYSTEM} MATCHES "Darwin")

if(EXPERIMENTAL_USE_FLUIDSYNTH)
    ADD_DEFINITIONS(-DEXPERIMENTAL_USE_FLUIDSYNTH)
    MESSAGE("Building using fluidsynth")
    SET( PB_BASE_SRCS MidiDeviceFluidSynth.cpp )

    if(FLUIDSYNTH_INPLACE_DIR)
        INCLUDE_DIRECTORIES(${FLUIDSYNTH_INPLACE_DIR}/include/)
        if(WIN32)
            LINK_LIBRARIES( ${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.dll.a)
        endif(WIN32)
        if(UNIX)
            LINK_LIBRARIES(${FLUIDSYNTH_INPLACE_DIR}/src/.libs/libfluidsynth.so)
        endif(UNIX)
    else(FLUIDSYNTH_INPLACE_DIR)
        pkg_check_modules(FLUIDSYNTH fluidsynth)
        if(NOT FLUIDSYNTH_FOUND)
            MESSAGE(FATAL_ERROR "FLUIDSYNTH was not found")
        endif(NOT FLUIDSYNTH_FOUND)
        LINK_LIBRARIES( fluidsynth)
    endif(FLUIDSYNTH_INPLACE_DIR)
endif(EXPERIMENTAL_USE_FLUIDSYNTH)

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${OPENGL_INCLUDE_DIR} ${FTGL_INCLUDE_DIR})

SET(PB_BASE_SRCS MidiFile.cpp MidiTrack.cpp Song.cpp Conductor.cpp Util.cpp
    Chord.cpp Tempo.cpp MidiDevice.cpp MidiDeviceRt.cpp ${PB_BASE_SRCS})
SET(PB_BASE_HDR MidiFile.h MidiTrack.h Song.h Conductor.h Rating.h Util.h
    Chord.h Tempo.h MidiDevice.h)


if(USE_JACK)
    # Check for Jack
    find_library(JACK_LIB jack)
    pkg_check_modules(JACK jack)
    if(NOT JACK_FOUND)
        MESSAGE(FATAL_ERROR "JACK was not found")
    endif(NOT JACK_FOUND)

    SET(JACK_INCLUDE_DIR ${JACK_INCLUDE_DIRS})
    SET(JACK_LIBRARY ${JACK_LIBRARIES})
    if(JACK_FOUND)
        ADD_DEFINITIONS(-D__UNIX_JACK__)
    endif(JACK_FOUND)
endif(USE_JACK)

if(USE_BUNDLED_RTMIDI)
    message(STATUS "Building using bundled rtmidi")
    add_compile_options("-DUSE_BUNDLED_RTMIDI")
    include_directories("3rdparty")
    set(PB_BASE_SRCS ${PB_BASE_SRCS} 3rdparty/rtmidi/RtMidi.cpp)
    set(PB_BASE_HDR ${PB_BASE_HDR} 3rdparty/rtmidi/RtMidi.h)
else()
    pkg_check_modules(RTMIDI REQUIRED rtmidi)
    include_directories(${RTMIDI_INCLUDE_DIRS})
    link_directories(${RTMIDI_LIBRARY_DIRS})
endif()

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
    pianobooster.rc
    images/pianobooster.ico
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

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# here we instruct CMake to build "pianobooster" executable from all of the source files
if(APPLE)
    ADD_EXECUTABLE( pianobooster MACOSX_BUNDLE ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} ${PIANOBOOSTER_RCS} )
endif(APPLE)

if(UNIX)
    ADD_EXECUTABLE( pianobooster ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} ${PIANOBOOSTER_RCS} )
endif(UNIX)

if(WIN32)
    ADD_EXECUTABLE( pianobooster WIN32 ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} ${PIANOBOOSTER_RCS} )
    SET_TARGET_PROPERTIES(pianobooster PROPERTIES LINK_FLAGS "-mwindows")
endif(WIN32)

qt5_use_modules(pianobooster Core Gui Widgets LinguistTools OpenGL Xml)

if(${CMAKE_VERSION} VERSION_LESS "3.13.0")
    message("Please consider to switch to CMake 3.13.0")
else()
    target_link_directories(pianobooster PUBLIC ${FTGL_LIBRARY_DIRS} ${JACK_LIBRARY_DIRS})
endif()
target_link_libraries (pianobooster Qt5::Widgets Qt5::Xml Qt5::OpenGL ${OPENGL_LIBRARIES} ${FTGL_LIBRARY} ${RTMIDI_LIBRARIES} ${JACK_LIBRARY})

INSTALL( FILES ../pianobooster.desktop DESTINATION share/applications )
INSTALL(TARGETS pianobooster RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(NOT NO_DOCS)
    INSTALL( FILES ../README.md DESTINATION share/doc/pianobooster/ )
    INSTALL( FILES ../doc/faq.md DESTINATION share/doc/pianobooster/ )
endif()

if(NOT NO_LICENSE)
    INSTALL( FILES ../license.txt DESTINATION share/licenses/pianobooster/ )
endif()

if(NOT NO_CHANGELOG)
    INSTALL( FILES ../Changelog.txt DESTINATION share/doc/pianobooster/ )
endif()

if(WITH_MAN)
    INSTALL( FILES ../pianobooster.6  DESTINATION share/man/man6/ )
endif()

INSTALL ( FILES ../icons/hicolor/32x32/pianobooster.png DESTINATION share/icons/hicolor/32x32/apps )
INSTALL ( FILES ../icons/hicolor/48x48/pianobooster.png DESTINATION share/icons/hicolor/48x48/apps )
INSTALL ( FILES ../icons/hicolor/64x64/pianobooster.png DESTINATION share/icons/hicolor/64x64/apps )

if(UNIX AND NOT APPLE)
    INSTALL ( FILES ../music/BoosterMusicBooks.zip DESTINATION ${DATA_DIR}/music )
endif(UNIX AND NOT APPLE)

if(WIN32)
    INSTALL( FILES ../music/BoosterMusicBooks.zip DESTINATION . )
endif(WIN32)

if(USE_FONT)
    INSTALL ( FILES ${USE_FONT} DESTINATION ${DATA_DIR}/fonts)
endif(USE_FONT)

if(NOT USE_SYSTEM_FONT)
    INSTALL ( FILES fonts/DejaVuSans.ttf DESTINATION ${DATA_DIR}/fonts)
endif(NOT USE_SYSTEM_FONT)

if(NOT NO_LANGS)
    ADD_SUBDIRECTORY(../translations ../build/translations)
endif(NOT NO_LANGS)

if(DEBUG_LOG_TRACE)
    ADD_DEFINITIONS(-DPB_LOG_TRACE)
endif()

if(DEBUG_LOG_TIMING)
    ADD_DEFINITIONS(-DPB_LOG_TIMING)
endif()

