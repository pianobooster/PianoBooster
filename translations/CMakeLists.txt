SET( TRANSLATIONS_FILES
     pianobooster_ca.ts
     pianobooster_cs.ts
     pianobooster_da.ts
     pianobooster_de.ts
     pianobooster_es.ts
     pianobooster_fr.ts
     pianobooster_gl.ts
     pianobooster_is.ts
     pianobooster_it.ts
     pianobooster_ja.ts
     pianobooster_nb.ts
     pianobooster_nl.ts
     pianobooster_pl.ts
     pianobooster_ru.ts
     pianobooster_sk.ts
     pianobooster_sv.ts
     pianobooster_uk.ts
     pianobooster_zh.ts
)

if(INSTALL_ALL_LANGS)
file (GLOB TRANSLATIONS_FILES "*.ts")
endif()

get_filename_component(full_path_blank_ts ${CMAKE_CURRENT_SOURCE_DIR}/pianobooster_blank.ts ABSOLUTE)
list(REMOVE_ITEM TRANSLATIONS_FILES ${full_path_blank_ts})

set(QT_PACKAGE_NAME Qt5 CACHE STRING "Name of the Qt package to use, set e.g. to \"Qt6\" to use Qt 6")
FIND_PACKAGE("${QT_PACKAGE_NAME}" REQUIRED COMPONENTS LinguistTools)

if(COMMAND QT_ADD_TRANSLATION)
    QT_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
else()
    QT5_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
endif()
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

IF (UNIX AND NOT APPLE)
    INSTALL( FILES ${QM_FILES} DESTINATION ${DATA_DIR}/translations )
    INSTALL( FILES langs.json DESTINATION ${DATA_DIR}/translations )
ENDIF (UNIX AND NOT APPLE)

IF (WIN32)
    INSTALL( FILES ${QM_FILES} DESTINATION . )
    INSTALL( FILES langs.json DESTINATION . )
ENDIF (WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/langs.json ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)
