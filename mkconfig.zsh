#!/usr/bin/env zsh

which () {
	# I need a proper and silent implementation of which.
	for i in $(echo "$PATH" | sed "s/:/ /g"); do
		if [ -e "$i/$1" ] && [ -x "$i/$1" ]; then
			echo -n "$i/$1"
			return 0
		fi
	done
	return 1
}

gnu () {
	# Just to know what is to gnu, and what is not bloated...
	${1:-echo} --version 2>&1 | grep -q GNU
}


procs_number=2

if [[ -e "/proc/cpuinfo" ]]; then
	procs_number="$(cat /proc/cpuinfo | grep processor | wc -l)"
fi

for tool in axel wget curl; do
	if [[ -n "$(which $tool)" ]]; then
		dl_tool="$tool"
	fi
done

for tool in bsdtar tar; do
	if [[ -n "$(which $tool)" ]]; then
		untar_tool=$tool
		if [[ "$untar_tool" == "tar" ]]; then
			untar_tool=gtar # GNU...
		fi
	fi
done

for tool in rpm bsdtar; do
	if [[ -n "$(which $tool)" ]]; then
		unrpm_tool=$tool
	fi
done

for tool in unzip bsdtar; do
	if [[ -n "$(which $tool)" ]]; then
		unzip_tool=$tool
	fi
done

cat <<-EOF
#
# /etc/pkg++.conf: pkg++(8) configuration
#

# For optimisation with CFLAGS, you can see 
# http://en.gentoo-wiki.com/wiki/Safe_Cflags
# It is *strongly recommended* to use a known -march= and not -march=native,
# because your package will be optimised for an unknown processor, which would
# be a problem to share them.

export CHOST=$MACHTYPE-$VENDOR-$OSTYPE

export CFLAGS="${CFLAGS:--O2 -march=@MARCH@ -pipe}"
export CXXFLAGS="\$CFLAGS"

export MAKE_OPTS="-j$(($procs_number+1))"

# Do NOT export USE directly, ZSH hates this.
USE=(nls man info free -binary)

# PKGMK_SOURCE_MIRRORS=()
PKGMK_SOURCE_DIR="/var/cache/pkg++/sources"
PKGMK_PACKAGE_DIR="/var/cache/pkg++/packages"
PKGMK_LOGS_DIR="/var/log/pkg++"
PKGMK_WORK_DIR="/tmp/work-\$name"
PKGMK_DOWNLOAD="yes"
PKGMK_IGNORE_FOOTPRINT="yes"
PKGMK_IGNORE_NEW="yes"
# PKGMK_NO_STRIP="no"
# PKGMK_WGET_OPTS=""
# PKGMK_CURL_OPTS=""
# PKGMK_COMPRESSION_MODE="gz"
# PKGMK_MAN_COMPRESSION="gz"

# Variable               # Values
PKGMK_DOWNLOAD_TOOL=${dl_tool:-curl} # curl wget axel
PKGMK_UNTAR_TOOL=${untar_tool:-gtar} # gtar bsdtar sltar
PKGMK_TARLIST_TOOL=${untar_tool:-gtar} # gtar bsdtar
PKGMK_UNRPM_TOOL=${unrpm_tool:-rpm} # rpm bsdtar
PKGMK_UNZIP_TOOL=${unzip_tool:-unzip} # unzip bsdtar

# These variables will be used for the packages construction.
PKGMK_PACKAGE_MANAGER=@PACKAGE_MANAGER@

# End of file
EOF

