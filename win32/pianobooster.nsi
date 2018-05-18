;NSIS Modern User Interface
;create the windows install exectuable
;Piano Booster

;--------------------------------
;Include Modern UI

    !include "MUI2.nsh"

;--------------------------------
;General

    ; The name of the installer
    Name "Piano Booster"

    ; The file to write
    OutFile "PianoBoosterInstall.exe"

    ; The default installation directory
    InstallDir "$PROGRAMFILES\Piano Booster"

    ; Registry key to check for directory (so if you install again, it will
    ; overwrite the old one automatically)
    InstallDirRegKey HKCU "Software\NSIS_PianoBooster" ""


    ;Request application privileges for Windows Vista
    RequestExecutionLevel admin

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

    !define MUI_ABORTWARNING

;--------------------------------
;Pages

    !insertmacro MUI_PAGE_WELCOME
    #!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
    #!insertmacro MUI_PAGE_COMPONENTS
    !insertmacro MUI_PAGE_DIRECTORY

    ;Start Menu Folder Page Configuration
    !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
    !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\NSIS_PianoBooster"
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

    !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

    !insertmacro MUI_PAGE_INSTFILES
    !insertmacro MUI_PAGE_FINISH

    !insertmacro MUI_UNPAGE_WELCOME
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES
    !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Install Section"

    SetOutPath "$INSTDIR"

    ; Put file there
    File FilesForRelease\pianobooster.exe
    #File FilesForRelease\libfluidsynth-1.dll
    File FilesForRelease\QtCore4.dll
    File FilesForRelease\QtGui4.dll
    File FilesForRelease\QtXml4.dll
    File FilesForRelease\QtOpenGL4.dll
    File FilesForRelease\mingwm10.dll
    File FilesForRelease\libgcc_s_dw2-1.dll

    CreateDirectory $INSTDIR\doc
    SetOutPath $INSTDIR\doc
    File FilesForRelease\README.txt
    File FilesForRelease\license.txt
    File FilesForRelease\gplv3.txt

    CreateDirectory "$DOCUMENTS\My Music"
    SetOutPath "$DOCUMENTS\My Music"
    File /r FilesForRelease\BoosterMusicBooks1

    SetOutPath $INSTDIR

    ; Write the installation path into the registry
    WriteRegStr HKCU SOFTWARE\NSIS_PianoBooster "" "$INSTDIR"

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

        ;Create shortcuts
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Piano Booster.lnk" "$INSTDIR\pianobooster.exe" "" "$INSTDIR\pianobooster.exe" 0
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\ReadMe.lnk" "$INSTDIR\doc\ReadMe.txt" "" "$INSTDIR\doc\ReadMe.txt" 0
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

    !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

    ;Language strings
;    LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

    ;Assign language strings to sections
;    !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;        !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
;    !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"


    ; Remove files and uninstaller
    Delete $INSTDIR\pianobooster.exe
    Delete $INSTDIR\mingwm10.dll
    #Delete $INSTDIR\libfluidsynth-1.dll
    Delete $INSTDIR\QtCore4.dll
    Delete $INSTDIR\QtGui4.dll
    Delete $INSTDIR\QtXml4.dll
    Delete $INSTDIR\QtOpenGL4.dll
    Delete $INSTDIR\mingwm10.dll
    Delete $INSTDIR\doc\README.txt
    Delete $INSTDIR\doc\license.txt
    Delete $INSTDIR\doc\gplv3.txt
    RMDir "$INSTDIR\doc"

    Delete $INSTDIR\uninstall.exe
    RMDir "$INSTDIR"

    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
    RMDir "$SMPROGRAMS\$StartMenuFolder"

    ; Remove registry keys
    DeleteRegKey /ifempty HKCU "Software\NSIS_PianoBooster"

SectionEnd
