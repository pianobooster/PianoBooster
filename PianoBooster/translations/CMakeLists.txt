SET( TRANSLATIONS_FILES
     pianobooster_ja.ts
     pianobooster_es.ts
     pianobooster_gl.ts
     pianobooster_fr.ts
)

QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

IF (UNIX AND NOT APPLE)
    INSTALL( FILES ${QM_FILES}
             DESTINATION share/games/pianobooster/translations )
ENDIF (UNIX AND NOT APPLE)

IF (WIN32)
    INSTALL( FILES ${QM_FILES}
             DESTINATION . )
ENDIF (WIN32)
