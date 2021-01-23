;NSIS Modern User Interface
;create the windows install executable
;Piano Booster

;--------------------------------
;Include Modern UI

    !include "MUI2.nsh"

;--------------------------------
;Defines

    !cd "..\..\"

    !define PB_BUILT_DIR "release\build"
    !if ! /FileExists "${PB_BUILT_DIR}\pianobooster.exe"
        !error "the release file '${PB_BUILT_DIR}\pianobooster.exe' not found"
    !endif


    ; The Env PB_INSTALL_FILES_PATH must be set to the path of all the addition release files
    ; trick to see if at compile time an environment variable exists
    ; see https://stackoverflow.com/questions/22149007/nsis-how-to-check-at-compile-time-if-an-environment-variable-exists
    !if "$%PB_INSTALL_FILES_PATH%" == "${U+24}%PB_INSTALL_FILES_PATH%"
        !error "the environmental variable PB_INSTALL_FILES_PATH is not set"
    !endif

    ; Put file there

    !define FILES_FOR_RELEASE $%PB_INSTALL_FILES_PATH%
    !echo "The FILES_FOR_RELEASE path is ${FILES_FOR_RELEASE}"

    !if ! /FileExists "${FILES_FOR_RELEASE}\Qt5Core.dll"
        !error "File '${FILES_FOR_RELEASE}\Qt5Core.dll' not found"
   !endif


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
    !insertmacro MUI_LANGUAGE "Afrikaans"
    !insertmacro MUI_LANGUAGE "Albanian"
    !insertmacro MUI_LANGUAGE "Arabic"
    !insertmacro MUI_LANGUAGE "Asturian"
    !insertmacro MUI_LANGUAGE "Basque"
    !insertmacro MUI_LANGUAGE "Belarusian"
    !insertmacro MUI_LANGUAGE "Bosnian"
    !insertmacro MUI_LANGUAGE "Breton"
    !insertmacro MUI_LANGUAGE "Bulgarian"
    !insertmacro MUI_LANGUAGE "Catalan"
    !insertmacro MUI_LANGUAGE "Corsican"
    !insertmacro MUI_LANGUAGE "Croatian"
    !insertmacro MUI_LANGUAGE "Czech"
    !insertmacro MUI_LANGUAGE "Danish"
    !insertmacro MUI_LANGUAGE "Dutch"
    !insertmacro MUI_LANGUAGE "English"
    !insertmacro MUI_LANGUAGE "Esperanto"
    !insertmacro MUI_LANGUAGE "Estonian"
    !insertmacro MUI_LANGUAGE "Farsi"
    !insertmacro MUI_LANGUAGE "Finnish"
    !insertmacro MUI_LANGUAGE "French"
    !insertmacro MUI_LANGUAGE "Galician"
    !insertmacro MUI_LANGUAGE "German"
    !insertmacro MUI_LANGUAGE "Greek"
    !insertmacro MUI_LANGUAGE "Hebrew"
    !insertmacro MUI_LANGUAGE "Hungarian"
    !insertmacro MUI_LANGUAGE "Icelandic"
    !insertmacro MUI_LANGUAGE "Indonesian"
    !insertmacro MUI_LANGUAGE "Irish"
    !insertmacro MUI_LANGUAGE "Italian"
    !insertmacro MUI_LANGUAGE "Japanese"
    !insertmacro MUI_LANGUAGE "Korean"
    !insertmacro MUI_LANGUAGE "Kurdish"
    !insertmacro MUI_LANGUAGE "Latvian"
    !insertmacro MUI_LANGUAGE "Lithuanian"
    !insertmacro MUI_LANGUAGE "Luxembourgish"
    !insertmacro MUI_LANGUAGE "Macedonian"
    !insertmacro MUI_LANGUAGE "Malay"
    !insertmacro MUI_LANGUAGE "Mongolian"
    !insertmacro MUI_LANGUAGE "Norwegian"
    !insertmacro MUI_LANGUAGE "NorwegianNynorsk"
    !insertmacro MUI_LANGUAGE "Pashto"
    !insertmacro MUI_LANGUAGE "Polish"
    !insertmacro MUI_LANGUAGE "Portuguese"
    !insertmacro MUI_LANGUAGE "PortugueseBR"
    !insertmacro MUI_LANGUAGE "Romanian"
    !insertmacro MUI_LANGUAGE "Russian"
    !insertmacro MUI_LANGUAGE "ScotsGaelic"
    !insertmacro MUI_LANGUAGE "Serbian"
    !insertmacro MUI_LANGUAGE "SerbianLatin"
    !insertmacro MUI_LANGUAGE "SimpChinese"
    !insertmacro MUI_LANGUAGE "Slovak"
    !insertmacro MUI_LANGUAGE "Slovenian"
    !insertmacro MUI_LANGUAGE "Spanish"
    !insertmacro MUI_LANGUAGE "SpanishInternational"
    !insertmacro MUI_LANGUAGE "Swedish"
    !insertmacro MUI_LANGUAGE "Tatar"
    !insertmacro MUI_LANGUAGE "Thai"
    !insertmacro MUI_LANGUAGE "TradChinese"
    !insertmacro MUI_LANGUAGE "Turkish"
    !insertmacro MUI_LANGUAGE "Ukrainian"
    !insertmacro MUI_LANGUAGE "Uzbek"
    !insertmacro MUI_LANGUAGE "Vietnamese"
    !insertmacro MUI_LANGUAGE "Welsh"
;--------------------------------
;Installer Sections

Section "Install Section"


    SetOutPath "$INSTDIR"

    SetOutPath $INSTDIR
    File "${PB_BUILT_DIR}\pianobooster.exe"
    File /r "${FILES_FOR_RELEASE}\*.*"



    CreateDirectory $INSTDIR\translations
    SetOutPath $INSTDIR\translations
    File "${PB_BUILT_DIR}\translations\*.qm"
    File "${PB_BUILT_DIR}\translations\langs.json"

    CreateDirectory $INSTDIR\fonts
    SetOutPath $INSTDIR\fonts
    File "${PB_BUILT_DIR}\fonts\*.ttf"
    
    CreateDirectory $INSTDIR\doc
    SetOutPath $INSTDIR\doc
    File license.txt
    File gplv3.txt

    CreateDirectory "$DOCUMENTS\My Music"
    SetOutPath "$DOCUMENTS\My Music"
    ; Temporary fix
    File /r "${PB_BUILT_DIR}\BoosterMusicBooks\BoosterMusicBooks4"

    SetOutPath $INSTDIR

    ; Write the installation path into the registry
    WriteRegStr HKCU SOFTWARE\NSIS_PianoBooster "" "$INSTDIR"

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

        ;Create shortcuts
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Piano Booster.lnk" "$INSTDIR\pianobooster.exe" "" "$INSTDIR\pianobooster.exe" 0
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
    RMDir /r "$INSTDIR"

    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
    RMDir "$SMPROGRAMS\$StartMenuFolder"

    ; Remove registry keys
    DeleteRegKey /ifempty HKCU "Software\NSIS_PianoBooster"

SectionEnd
