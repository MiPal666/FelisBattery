Name:       harbour-felisbattery
Summary:    Battery and charger diagnostics for Jolla Phone 2026
Version:    0.1.2
Release:    2
Group:      Qt/Qt
License:    GPL-3.0-or-later
URL:        https://github.com/MiPal666/FelisBattery
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9

BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils

%description
FelisBattery displays battery state and detailed charger diagnostics.
Detailed charger-path diagnostics are designed and tested for Jolla Phone 2026.
Other devices are unsupported but may provide basic battery information.

%prep
%autosetup -n %{name}-%{version}

%build
%qmake5
%make_build

%install
%qmake5_install

desktop-file-install --delete-original \
    --dir %{buildroot}%{_datadir}/applications \
    %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
