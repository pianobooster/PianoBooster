%define build_with_cmake 1

%define orig_name PianoBooster

%if 0%{?suse_version}
%define qmake_qt5 qmake-qt5
%endif

Name:           pianobooster
Version:        0.7.1
Release:        %mkrel 1
Summary:        A MIDI file player that teaches you how to play the piano
%if 0%{?mageia}
Group:          Sound/Midi
%endif
%if 0%{?fedora}
Group:          Applications/Sound
%endif
%if 0%{?suse_version}
Group:          Productivity/Multimedia/Sound/Midi
%endif
%if 0%{?mageia} || 0%{?fedora}
License:        GPLv3+
%endif
%if 0%{?suse_version}
License:        GPL-3.0-or-later
%endif
Url:            https://github.com/captnfab/PianoBooster
Source0:        %{orig_name}-%{version}.tar.gz

%if %{build_with_cmake}
BuildRequires:  cmake
%endif
BuildRequires:  pkgconfig(alsa)
BuildRequires:  pkgconfig(freetype2)
BuildRequires:  pkgconfig(ftgl)
BuildRequires:  pkgconfig(gl)
BuildRequires:  pkgconfig(glu)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Help)
BuildRequires:  pkgconfig(Qt5OpenGL)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Xml)
BuildRequires:  pkgconfig(rtmidi)
BuildRequires:  pkgconfig(fluidsynth)
BuildRequires:  hicolor-icon-theme

%if 0%{?mageia}
Requires:       fonts-ttf-dejavu
%endif
%if 0%{?suse_version}
Requires:       dejavu-fonts
%endif
%if 0%{?fedora}
Requires:       dejavu-sans-fonts
%endif
Requires:       unzip
Requires:       hicolor-icon-theme

#Recommends:     %%{name}-fluidsynth
%if 0%{?mageia}
Recommends:     qttranslations5
%endif
%if 0%{?suse_version}
Recommends:     libqt5-qttranslations
%endif
%if 0%{?fedora}
Recommends:     qt5-qttranslations
%endif

%description
A MIDI file player/game that displays the musical notes AND teaches you how
to play the piano.

PianoBooster is a fun way of playing along with a musical accompaniment and
at the same time learning the basics of reading musical notation.
The difference between playing along to a CD or a standard MIDI file
is that PianoBooster listens and reacts to what you are playing on a
MIDI keyboard.

To run Piano Booster you need a MIDI Piano Keyboard and a MIDI interface
for the PC. If you don't have a MIDI keyboard you can still try out
PianoBooster, using the PC keyboard ('x' is middle C), but a MIDI piano
is really recommended.

%files
%doc README.md Changelog.txt doc/faq.md
%license license.txt
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%dir %{_datadir}/games/%{name}
%dir %{_datadir}/games/%{name}/music
%dir %{_datadir}/games/%{name}/translations
%{_datadir}/games/%{name}/music/*.zip
%{_datadir}/games/%{name}/translations/%{name}*.qm
%{_datadir}/games/%{name}/translations/music*.qm
%{_datadir}/games/%{name}/translations/*.json
%{_mandir}/man6/%{name}.6*

#----------------------------------------------------------------------------

%if 0%{?mageia}
Group:          Sound/Midi
%endif
%if 0%{?fedora}
Group:          Applications/Sound
%endif
%if 0%{?suse_version}
Group:          Productivity/Multimedia/Sound/Midi
%endif

Requires:       %{name} = %{version}-%{release}

%if 0%{?mageia} || 0%{?fedora}
Requires:       libnotify
%endif
%if 0%{?suse_version}
Requires:       libnotify-tools
%endif

#----------------------------------------------------------------------------

%prep
%autosetup -p1 -n %{orig_name}-%{version}

%build
%if %{build_with_cmake}
%cmake \
       -DUSE_SYSTEM_FONT=ON \
       -DNO_DOCS=ON \
       -DNO_LICENSE=ON \
       -DNO_CHANGELOG=ON \
       -DWITH_MAN=ON
%else
%qmake_qt5 \
       USE_SYSTEM_FONT=ON \
       NO_DOCS=ON \
       NO_LICENSE=ON \
       NO_CHANGELOG=ON \
       WITH_MAN=ON
%endif
%make_build

%install
%if %{build_with_cmake}
%make_install -C build
%else
%make_install INSTALL_ROOT=%{buildroot}
%endif
