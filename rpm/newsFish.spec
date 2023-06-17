# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.27
# 

Name:       newsFish

# >> macros
# << macros

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    newsFish ownCloud mobile news client for Sailfish
Version:    0.3
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Requires:       qt-runner
Requires:       opt-qt5-qtwayland >= 5.15.8
Requires:       opt-qt5-qtquickcontrols2 >= 5.15.8
Requires:       opt-qt5-qtwayland >= 5.15.8
Requires:       opt-qt5-sfos-maliit-platforminputcontext
BuildRequires:  opt-qt5-qtdeclarative-devel >= 5.15.8
BuildRequires:  opt-qt5-qtquickcontrols2-devel >= 5.15.8
BuildRequires:  opt-qt5-qtbase-devel >= 5.15.8
BuildRequires:  opt-kf5-rpm-macros
BuildRequires:  opt-extra-cmake-modules
BuildRequires:  gcc-c++
BuildRequires:  opt-kf5-kirigami2-devel
BuildRequires:  opt-kf5-kirigami-addons
BuildRequires:  qqc2-breeze-style
BuildRequires:  opt-kf5-ki18n-devel
BuildRequires:  opt-kf5-kcoreaddons-devel

BuildRequires:  desktop-file-utils
%{?opt_qt5_default_filter}

%description
Offline news reader for Kirigami and Next Cloud

%prep
%autosetup -n %{name}-%{version}

%build
export QTDIR=%{_opt_qt5_prefix}
touch .git

mkdir -p build
pushd build

%_opt_cmake_kf5 ../ \
                -DKDE_INSTALL_BINDIR:PATH=/usr/bin \
                -DCMAKE_INSTALL_PREFIX:PATH=/usr/ \
                -DSAILFISHOS=1
%make_build
popd

%install
rm -rf %{buildroot}
pushd build
make DESTDIR=%{buildroot} install
popd

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

mkdir -p %{buildroot}%{_datadir}/icons/hicolor/86x86/apps/
cp newsFish.png %{buildroot}%{_datadir}/icons/hicolor/86x86/apps/

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}/qml
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
