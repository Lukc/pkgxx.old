
pkgmake() {
	# FIXME: DEPRECATION
	if [[ -n "$MAKE" ]]; then
		warning
		warning "Using \$MAKE is deprecated to define a \`make' implementation."
		warning "Use \$make instead."
		warning
	fi
	
	: ${make=$(which make)}   \
	  ${make:=$(which gmake)} \
	  ${make:=$(which bmake)} \
	  ${make:=$(which pmake)} \
	  ${make:=$MAKE}
	
	if ! USE=(${supports[@]}) use multithread; then
		# Note: fucking preprocessor
		while [[ "${make_opts[@]}" =~ -j[0-9] ]]; do
			make_opts=(${make_opts[@]/-j[0-9]})
		done
	fi
	
	if [[ -n "$make" ]]; then
		$make \
			${CC:+CC=$CC}              \
			${CXX:+CXX=$CXX}           \
			${CPP:+CPP=$CPP}           \
			${AS:+AS=$AS}              \
			${AR:+AR=$AR}              \
			${LD:+LD=$LD}              \
			${NM:+NM=$NM}              \
			${RANLIB:+RANLIB=$RANLIB}  \
			${STRIP:+STRIP=$STRIP}     \
			${make_opts[@]:-$MAKE_OPTS} $@
	else
		die "No make implementation available."
	fi
}

sedi() {
	# Emulates the sed -i feature.
	# Usage: sedi sed-script files
	# Note: Stollen from c4o. (Cruxports for OpenBSD)
	TMP1=$(mktemp sedi.XXXXXXXXXX) || die "mktemp [sedi] failed."
	REGEX="$1"
	shift
	if istrue $sed_gnu; then
		$sed "$REGEX" -i $@
	else
		# Useless to try to use -isuffix, it could destroy a file 
		# that already exists, even if it is very few probable, or 
		# would need more code…
		for i in $*; do
			sed "$REGEX" "$i" > $TMP1 || die "sed '$REGEX' '$i' [sedi] or redirection to '$TMP1' failed."
			# Preserve permissions, owner, etc.
			cat $TMP1 > "$i" || die "cat '$TMP1' [sedi] or redirection to '$i' failed."
		done
	fi
	rm $TMP1
	unset TMP1
}

replace() {
	# Replaces pattern $1 by pattern $2 in file $3.
	sedi "s|${1//|/\|}|${2//|/\|}|" "$3"
}

wcat() {
	# Download and print a file to stdout.
	local i
	for i in $@; do
		if [[ "$(type ${PKGMK_DOWNLOAD_TOOL}:cat)" != none ]]; then
			${PKGMK_DOWNLOAD_TOOL}:cat $i
		else
			curl:cat $i
		fi
	done
}

die() {
	# Display a given error message.
	local msg="$1"
	shift 1

	error "${?:+!$?!} $msg" $@
	exit 1
}

use_enable() {
	# For use with autotools. $(use_enable feature) will return
	# --enable-feature if the “feature” use flag is set, or
	# --disable-feature if it is not.
	# 
	# Note: $2 can give the name of the feature of the configure
	#       script, if it is not the same as the use flag.
	local flag=$1
	local feature=${2:-$flag}
	local value=${3:+=$3}
	
	if [[ -z "$flag" ]]; then
		error "use_enable() used without a parameter."
		return 1
	fi
	
	if use $flag; then
		echo -n "--enable-$feature$value"
	else
		echo -n "--disable-$feature"
	fi
}

use_with() {
	# Clone of use_enable(). See the function just before.
	local flag=$1
	local feature=${2:-$flag}
	local value=${3:+=$3}
	
	if [[ -z "$flag" ]]; then
		error "use_with() used without a parameter."
		return 1
	fi
	
	if use $flag; then
		echo -n "--with-$feature$value"
	else
		echo -n "--without-$feature"
	fi
}

pkgsplit() {
	local split="${1}"
	shift 1
	cd $PKG
	
	# We use tar to copy a complete tree.
	for FILE in $@; do
		if [[ -e ".$FILE" ]]; then
			tar c ".$FILE" | (cd $SPLITS/$split ; tar x)
			rm -rf ".$FILE"
		fi
	done
}

target_arch () {
	# This function return the architecture depending on the triplet.
	# The triplet may be the CHOST or the MACHTYPE, thus allowing 
	# cross-compilation.
	if [[ -n "$CHOST" ]]; then
		echo $CHOST | cut -d- -f 1
	else
		echo $MACHTYPE # We use the architecture zsh was built with…
	fi
}

target_kernel () {
	# Same as target_arch(), but for the OSTYPE.
	if [[ -n "$CHOST" ]]; then
		echo $CHOST | cut -d- -f 3
	else
		echo $OSTYPE | cut -d- -f 1
	fi
}

target_libc () {
	# Same as target_arch() and target_kernel(), but for the last part
	# of the OSTYPE, which is not always present. If it is not present, 
	# it returns the third part of the triplet, which is often 
	# representative of the desired information. The “libc” and the 
	# kernel are often the same on *BSD systems, which have a *BSD 
	# kernel and a *BSD libc, for example. It is the same for many other
	# OSes.
	local LIBC
	
	if [[ -n "$CHOST" ]]; then
		LIBC=$(echo "$CHOST" | cut -d- -f 4)

		if [[ -z "$LIBC" ]]; then
			LIBC=$(echo "$CHOST" | cut -d- -f 3)
		fi
	else
		# If OSTYPE is only the OS (no libc), it will be given instead anyway
		LIBC=$(echo $OSTYPE | cut -d- -f 2)
	fi
	
	echo "$LIBC"
}

pm_arch () {
	# This function returns a correct architecture for the current
	# package manager, using the triplet_arch() function.
	# 
	# No-arch packages must be of the form -noarch, instead of being of 
	# the form -x86 or whatever.
	local TARGET_ARCH=$(target_arch)
	local TARGET_KERNEL=$(target_kernel)
	
	if has "$PKGMK_PACKAGE_MANAGER" ${PKGMK_PM_NOARCH_SUPPORT[@]} && \
	  (has no-arch ${archs[@]} || has no-kernel ${kernels[@]})
	then
		TARGET_ARCH=noarch ;
	fi
	
	if [[ "$(type ${PKGMK_PACKAGE_MANAGER}:arch)" != none ]]; then
		ARCH="${TARGET_ARCH}" KERNEL="${TARGET_KERNEL}" \
			${PKGMK_PACKAGE_MANAGER}:arch
	else
		echo "${TARGET_ARCH}" 
	fi
}

pm_kernel () {
	# Same as pm_arch() but for kernels.
	local TARGET_KERNEL=$(target_kernel)
	case ${TARGET_KERNEL} in
		freebsd*) TARGET_KERNEL=FreeBSD ;;
		openbsd*) TARGET_KERNEL=OpenBSD ;;
		netbsd*) TARGET_KERNEL=NetBSD ;;
		linux) TARGET_KERNEL=Linux ;;
		# Feel free to submit patches to complete this…
	esac
	echo ${TARGET_KERNEL}
}

lastver () {
	local lastver="$1"
	shift 1
	for i in $@; do
		if vercmp "$i" ">" "$lastver"; then
			lastver="$i"
		fi
	done
	echo "$lastver"
}

lasttar() {
	local tarname="$2"
	lastver $(
		wcat "$1" | \
			grep "${tarname:=$name}-.*\.tar\(gz\|bz2\|xz\|lzma\|lzo\)*" | \
	)
}

#undef make_var

# vim:set syntax=sh shiftwidth=4 tabstop=4:
