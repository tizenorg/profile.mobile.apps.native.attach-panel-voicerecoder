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
BuildRequires: pkgconfig(libtzplatform-config)

%description
Description: attach-panel-voicerecorder UG

%prep
%setup -q

%build

%define _app_license_dir          %{TZ_SYS_SHARE}/license

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_ -mfpu=neon"
%endif

%cmake . -DCMAKE_INSTALL_PREFIX=%{TZ_SYS_RO_UG} \
	-DARCH=%{ARCH} \
	-DTZ_SYS_RO_PACKAGES=%{TZ_SYS_RO_PACKAGES}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%make_install

mkdir -p %{buildroot}%{_app_license_dir}
cp LICENSE %{buildroot}%{_app_license_dir}/attach-panel-voicerecorder

%post
mkdir -p /usr/ug/bin/
ln -sf /usr/bin/ug-client %{TZ_SYS_RO_UG}/bin/attach-panel-voicerecorder
%postun

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{TZ_SYS_RO_UG}/lib/libug-attach-panel-voicerecorder.so*
%{TZ_SYS_RO_UG}/res/edje/ug-attach-panel-voicerecorder/*
%{TZ_SYS_RO_UG}/res/images/attach-panel-voicerecorder/*
%{TZ_SYS_RO_UG}/res/locale/*
%{TZ_SYS_RO_PACKAGES}/attach-panel-voicerecorder.xml
%{TZ_SYS_SHARE}/license/attach-panel-voicerecorder

