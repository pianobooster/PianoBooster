cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# set project's name
PROJECT( pianobooster )


ADD_SUBDIRECTORY(src build)
ADD_SUBDIRECTORY(translations build/translations)
ADD_SUBDIRECTORY(music build/music)
