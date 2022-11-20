Summary: Leibnitz provides electronic paper for calculations and graphs.
Name: %app_name
Version: %pkg_version
Release: 1
License: GPL
Group: Applications/Scientific
Source: %pkg_name
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre, curl

%description
Leibnitz provides electronic paper for calculations and graphs.

%prep
%setup

%install

%define leibnitz_doc_dir  /usr/local/share/doc/leibnitz
%define gnome_app_path    /usr/local/share/applications
%define gnome_icon_path   /usr/local/share/pixmaps

./install $RPM_BUILD_ROOT/usr/local

%files

%docdir %leibnitz_doc_dir

/usr/local/bin/leibnitz
%leibnitz_doc_dir

%gnome_app_path/leibnitz.desktop
%gnome_icon_path/leibnitz.png
