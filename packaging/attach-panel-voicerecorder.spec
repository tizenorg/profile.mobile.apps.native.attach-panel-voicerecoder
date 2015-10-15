%define _appdir	%{_prefix}/apps
%define _ugdir	%{_prefix}/ug
%define _datadir %{_prefix}/share
%define _sharedir /opt/usr/media/.iv
Name:       attach-panel-voicerecorder
Summary:    voicerecorder UX
Version:    0.1.0
Release:    1
Group:      Applications
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: gettext-tools
BuildRequires: edje-tools

BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(capi-media-recorder)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(capi-content-media-content)

%description
Description: voicerecorder UG

%prep
%setup -q

%build

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_ -mfpu=neon"
%endif

%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_DATA_DIR=%{_datadir} -DARCH=%{ARCH}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
if [ ! -d %{buildroot}/opt/usr/apps/attach-panel-voicerecorder/data ]
then
        mkdir -p %{buildroot}/opt/usr/apps/attach-panel-voicerecorder/data
fi

%make_install

mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}%{_sharedir}
cp LICENSE %{buildroot}/usr/share/license/attach-panel-voicerecorder

%post
mkdir -p %{_prefix}/apps/%{name}/bin/
ln -sf %{_prefix}/bin/ug-client %{_prefix}/apps/%{name}/bin/%{name}
%postun

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_appdir}/%{name}/lib/ug/libattach-panel-voicerecorder.so*
%{_prefix}/ug/res/edje/attach-panel-voicerecorder/*
%{_prefix}/ug/res/images/attach-panel-voicerecorder/*
%{_prefix}/ug/res/locale/*
%{_datadir}/packages/attach-panel-voicerecorder.xml
%{_datadir}/license/attach-panel-voicerecorder

