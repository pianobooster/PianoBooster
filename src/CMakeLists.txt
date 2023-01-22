# Cmake File for Piano Booster

option(WITH_INTERNAL_FLUIDSYNTH "Build with an internal FluidSynth sound generator" ON)
option(USE_FTGL "Build with ftgl for notes localization" ON)
option(USE_SYSTEM_FONT "Build with system font" OFF)
option(USE_JACK "Build with Jack (Only required for BSD Unix)" OFF)
if(${CMAKE_SYSTEM} MATCHES "Linux")
   option(USE_BUNDLED_RTMIDI "Build with bundled rtmidi (for older distributions only)" OFF)
else()
   option(USE_BUNDLED_RTMIDI "Build with bundled rtmidi" ON)
endif()

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(WIN32)
    MESSAGE("GUI system is WIN32 ${CMAKE_GENERATOR}")
endif(WIN32)

# set project's name
PROJECT( pianobooster )

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)

# enable warnings
add_compile_options(-Wall)


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

set(OpenGL_GL_PREFERENCE "GLVND")
FIND_PACKAGE( OpenGL REQUIRED )

if(NOT WIN32 OR MINGW)
   # the `pkg_check_modules` function is created with this call
   include(FindPkgConfig)
   FIND_PACKAGE( PkgConfig REQUIRED )
endif()

if(USE_FTGL)
   if(NOT WIN32)
      pkg_check_modules(FTGL ftgl)
      if(NOT FTGL_FOUND)
         MESSAGE(FATAL_ERROR "FTGL was not found")
      endif(NOT FTGL_FOUND)
      SET(FTGL_INCLUDE_DIR ${FTGL_INCLUDE_DIRS})
      SET(FTGL_LIBRARY ${FTGL_LIBRARIES})
   else()
      FIND_PACKAGE(FTGL REQUIRED)

      # add include path for freetype2 as ftgl uses freetype2 headers but doesn't pull in the include path automatically
      if (COMMAND pkg_check_modules)
          pkg_check_modules(FREETYPE2 freetype2)
          if(FREETYPE2_FOUND)
              LIST(APPEND FTGL_INCLUDE_DIR "${FREETYPE2_INCLUDE_DIRS}")
          endif ()
      endif ()
   endif()
else(USE_FTGL)
    add_compile_options("-DNO_USE_FTGL")
endif(USE_FTGL)

if(NO_LANGS)
    add_compile_options("-DNO_LANGS")
endif()

# Finds Qt libraries
set(QT_PACKAGE_NAME Qt5 CACHE STRING "Name of the Qt package to use, set e.g. to \"Qt6\" to use Qt 6")
set(QT_COMPONENTS Core Gui Widgets LinguistTools OpenGL Xml)
set(QT_LIBS ${QT_PACKAGE_NAME}::Core ${QT_PACKAGE_NAME}::Gui ${QT_PACKAGE_NAME}::Widgets ${QT_PACKAGE_NAME}::Xml ${QT_PACKAGE_NAME}::OpenGL)
if (QT_PACKAGE_NAME STREQUAL Qt6)
    list(APPEND QT_COMPONENTS OpenGLWidgets Core5Compat)
    list(APPEND QT_LIBS ${QT_PACKAGE_NAME}::OpenGLWidgets ${QT_PACKAGE_NAME}::Core5Compat)
endif ()
FIND_PACKAGE( "${QT_PACKAGE_NAME}" REQUIRED COMPONENTS ${QT_COMPONENTS})

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
    ADD_DEFINITIONS(-std=c++11)
    LINK_LIBRARIES("-framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework OpenGL")
endif(${CMAKE_SYSTEM} MATCHES "Darwin")

if(WITH_INTERNAL_FLUIDSYNTH)
    if(NOT WIN32)
        pkg_check_modules(FLUIDSYNTH fluidsynth)

        if(NOT FLUIDSYNTH_FOUND)
           MESSAGE(FATAL_ERROR "FLUIDSYNTH was not found")
        endif(NOT FLUIDSYNTH_FOUND)
        INCLUDE_DIRECTORIES(${FLUIDSYNTH_INCLUDE_DIRS})
        SET(FLUIDSYNTH_LIBRARY ${FLUIDSYNTH_LIBRARIES})
    else()
        # find FluidSynth and link against its imported target
        FIND_PACKAGE(FluidSynth REQUIRED)
        SET(FLUIDSYNTH_LIBRARY FluidSynth::libfluidsynth)

        # find additional dependencies that are pulled in by the imported target but not automatically made available
        FIND_PACKAGE(OpenMP REQUIRED)
        if (COMMAND pkg_check_modules)
            pkg_check_modules(GLIB IMPORTED_TARGET glib-2.0 gthread-2.0)
            pkg_check_modules(LIBSNDFILE IMPORTED_TARGET sndfile)
            pkg_check_modules(PORTAUDIO IMPORTED_TARGET portaudio-2.0)
            pkg_check_modules(READLINE IMPORTED_TARGET readline)
        endif ()
    endif()
    ADD_DEFINITIONS(-DWITH_INTERNAL_FLUIDSYNTH)
    MESSAGE("Building with internal fluidsynth")
    SET( PB_BASE_SRCS MidiDeviceFluidSynth.cpp )
endif(WITH_INTERNAL_FLUIDSYNTH)

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
    pkg_check_modules(RTMIDI rtmidi)
    if(NOT RTMIDI_FOUND)
        MESSAGE(FATAL_ERROR "rtmidi not found (Try building with option USE_BUNDLED_RTMIDI=ON)")
    endif(NOT RTMIDI_FOUND)
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
    # the property added the pianobooster icon to Info.plist
    set(MACOSX_BUNDLE_ICON_FILE pianobooster.icns)

    # Tells CMake where to find and install the pianobooster icon
    set(pianobooster_ICON ${CMAKE_CURRENT_SOURCE_DIR}/../icons/MacOS/pianobooster.icns)
    set_source_files_properties(${pianobooster_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(pianobooster_TRANSLATIONS ${CMAKE_CURRENT_BINARY_DIR}/translations/langs.json)
    set_source_files_properties(${pianobooster_TRANSLATIONS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/translations")

    set(pianobooster_MUSIC ${CMAKE_CURRENT_SOURCE_DIR}/../music/BoosterMusicBooks.zip)
    set_source_files_properties(${pianobooster_MUSIC} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/music")
    set(pianobooster_FONT ${CMAKE_CURRENT_SOURCE_DIR}/fonts/DejaVuSans.ttf)
    set_source_files_properties(${pianobooster_FONT} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/fonts")

    ADD_EXECUTABLE( pianobooster MACOSX_BUNDLE
        ${pianobooster_ICON} ${pianobooster_TRANSLATIONS}
        ${pianobooster_MUSIC} ${pianobooster_FONT}
        ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} ${PIANOBOOSTER_RCS} )

    add_custom_target(install-translations cp -R -v ${CMAKE_CURRENT_BINARY_DIR}/translations/*.qm ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/translations)

endif(APPLE)

if(UNIX AND NOT APPLE)
    ADD_EXECUTABLE( pianobooster ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} ${PIANOBOOSTER_RCS} )
endif(UNIX AND NOT APPLE)

if(WIN32)
    ADD_EXECUTABLE( pianobooster ${PIANOBOOSTER_SRCS}
        ${PIANOBOOSTER_MOC_SRCS} ${PIANOBOOSTER_RC_SRCS}
        ${PIANOBOOSTER_UI_HDRS} ${PIANOBOOSTER_RCS} )
    SET_TARGET_PROPERTIES(pianobooster PROPERTIES LINK_FLAGS "-mwindows")
endif(WIN32)

if(${CMAKE_VERSION} VERSION_LESS "3.13.0")
    message("Please consider to switch to CMake 3.13.0")
else()
    target_link_directories(pianobooster PUBLIC ${FTGL_LIBRARY_DIRS} ${JACK_LIBRARY_DIRS} ${FLUIDSYNTH_LIBRARY_DIRS})
endif()
target_link_libraries (pianobooster ${QT_LIBS} ${OPENGL_LIBRARIES} ${FTGL_LIBRARY} ${RTMIDI_LIBRARIES} ${JACK_LIBRARY} ${FLUIDSYNTH_LIBRARY} ${RTMIDI_LIBRARY})

if(NOT APPLE)
    INSTALL( FILES ../pianobooster.desktop DESTINATION share/applications )
    INSTALL(TARGETS pianobooster RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif(NOT APPLE)

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

if(NOT USE_SYSTEM_FONT)
    configure_file(fonts/DejaVuSans.ttf ${CMAKE_CURRENT_BINARY_DIR}/fonts/DejaVuSans.ttf COPYONLY)
    INSTALL ( FILES fonts/DejaVuSans.ttf DESTINATION ${DATA_DIR}/fonts/)
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

# allow enabling useful warnings (explicitly disabling not useful ones) and allow treating warnings them as errors
# note: code copied from: https://github.com/Martchus/cpp-utilities/blob/d8c38699ba41b048c61662786c69eb02ff7eaca7/cmake/modules/BasicConfig.cmake
option(ENABLE_WARNINGS "adds additional compiler flags to enable useful warnings" OFF)
set(CLANG_WARNINGS
    -Wall
    -Wextra # reasonable and standard
    -Wshadow=local # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard
                       # to track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual function
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wnull-dereference # warn if a null dereference is detected
    -Wdouble-promotion # warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output (ie printf)
    -Wno-pedantic # warn NOT if non-standard C++ is used (some vendor extensions are very useful)
    -Wno-missing-field-initializers # warn NOT about missing field initializers
    -Wno-useless-cast # warn NOT about useless cases (this is sometimes very useful in templates)
    -Wno-unused-const-variable # warn NOT about unused constants (usually used in other compile unit)
    -Wno-unknown-warning-option # warn NOT about unknown warning options (depending on compiler/version not all are
                                # available)
)
set(GCC_WARNINGS
    ${CLANG_WARNINGS}
    -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
)
if (ENABLE_WARNINGS)
    if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        target_compile_options(pianobooster PRIVATE ${CLANG_WARNINGS})
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(pianobooster PRIVATE ${GCC_WARNINGS})
    else ()
        message(AUTHOR_WARNING "Enabling warnings is not supported for compiler '${CMAKE_CXX_COMPILER_ID}'.")
    endif ()
endif ()
option(TREAT_WARNINGS_AS_ERRORS "adds additional compiler flag to treat warnings as errors" "${ENABLE_DEVEL_DEFAULTS}")
if (TREAT_WARNINGS_AS_ERRORS)
    if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(pianobooster PRIVATE -Werror)
    else ()
        message(AUTHOR_WARNING "Treating warnings as errors is not supported for compiler '${CMAKE_CXX_COMPILER_ID}'.")
    endif ()
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../music/BoosterMusicBooks.zip ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)

