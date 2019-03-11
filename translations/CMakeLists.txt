SET( TRANSLATIONS_FILES
     pianobooster_ca.ts
     pianobooster_cs.ts
     pianobooster_de.ts
     pianobooster_es.ts
     pianobooster_fr.ts
     pianobooster_gl.ts
     pianobooster_ja.ts
     pianobooster_nl.ts
     pianobooster_ru.ts
     music_ru.ts
)

if(INSTALL_ALL_LANGS)
file (GLOB TRANSLATIONS_FILES "*.ts")   
endif()

get_filename_component(full_path_blank_ts ${CMAKE_CURRENT_SOURCE_DIR}/pianobooster_blank.ts ABSOLUTE)
list(REMOVE_ITEM TRANSLATIONS_FILES ${full_path_blank_ts})

get_filename_component(full_path_blank_ts ${CMAKE_CURRENT_SOURCE_DIR}/music_blank.ts ABSOLUTE)
list(REMOVE_ITEM TRANSLATIONS_FILES ${full_path_blank_ts})

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS LinguistTools)

QT5_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

IF (UNIX AND NOT APPLE)
    INSTALL( FILES ${QM_FILES} DESTINATION share/games/pianobooster/translations )
    INSTALL( FILES langs.json DESTINATION share/games/pianobooster/translations )
ENDIF (UNIX AND NOT APPLE)

IF (WIN32)
    INSTALL( FILES ${QM_FILES} DESTINATION . )
    INSTALL( FILES langs.json DESTINATION . )
ENDIF (WIN32)

