Name:           pianobooster
Version:        0.7.0
Release:        %mkrel 1
Summary:        A MIDI file player that teaches you how to play the piano
%if 0%{?mageia}
Group:          Sound/Midi
%endif
%if 0%{?suse}
Group:          Productivity/Multimedia/Sound/Midi
%endif
License:        GPLv3+
Url:            https://github.com/captnfab/PianoBooster
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
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
%if 0%{?suse}
Requires:       dejavu-fonts
%endif
Requires:       unzip
Requires:       hicolor-icon-theme

Recommends:     %{name}-timidity
#Recommends:     %%{name}-fluidsynth
%if 0%{?mageia}
Recommends:     qttranslations5
%endif
%if 0%{?suse}
Recommends:     libqt5-qttranslations
%endif

%description
A MIDI file player/game that displays the musical notes AND teaches you how
to play the piano.

PianoBooster is a fun way of playing along with a musical accompaniment and
at the same time learning the basics of reading musical notation.
The difference between playing along to a CD or a standard MIDI file
is that PianoBooster listens and reacts to what you are playing on a
MIDI keyboard.

%files
%doc README.md ReleaseNotes.txt
%license license.txt
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/games/%{name}
%{_mandir}/man6/%{name}.6*

#----------------------------------------------------------------------------

%package        timidity
Summary:        Wrapper to launch PianoBooster with TiMidity as MIDI sequencer
Group:          Sound/Midi

Requires:       %{name} = %{version}-%{release}
%if 0%{?mageia}
Requires:       TiMidity++
%endif
%if 0%{?suse}
Requires:       timidity
%endif

%description    timidity
This package contains a wrapper script to launch PianoBooster together with
TiMidity in ALSA server mode. This makes it possible to play the MIDI files
even without a plugged-in MIDI keyboard.

%files          timidity
%{_bindir}/%{name}-timidity
%{_datadir}/applications/%{name}-timidity.desktop

#----------------------------------------------------------------------------

%package        fluidsynth
Summary:        Wrapper to launch PianoBooster with FluidSynth as MIDI sequencer
Group:          Sound/Midi

Requires:       %{name} = %{version}-%{release}
Requires:       fluidsynth
Requires:       fluid-soundfont-gm
Requires:       fluid-soundfont-gs
%if 0%{?mageia}
Requires:       libnotify
%endif
%if 0%{?suse}
Requires:       libnotify-tools
%endif

%description    fluidsynth
This package contains a wrapper script to launch PianoBooster together with
FluidSynth in ALSA server mode. This makes it possible to play the MIDI files
even without a plugged-in MIDI keyboard.

%files          fluidsynth
%{_bindir}/%{name}-fluidsynth
%{_datadir}/applications/%{name}-fluidsynth.desktop

#----------------------------------------------------------------------------

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%cmake \
       -DUSE_SYSTEM_FONT=ON \
       -DNO_DOCS=ON \
       -DINSTALL_ALL_LANGS=ON \
       -DUSE_SYSTEM_RTMIDI=ON \
       -DWITH_MAN=ON \
       -DWITH_TIMIDITY=ON \
       -DWITH_FLUIDSYNTH=ON \
       -DUSE_FLUIDSYNTH=ON
%make_build

%install
%make_install -C build
