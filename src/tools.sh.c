
pkgmake() {
	/*
	 * That avoids to use a non-gnu make, which could cause problems on 
	 * some ports.
	 */
	GMAKE \
		CC="$CC" CXX="$CXX" CPP="$CPP" AS="$AS" AR="$AR" LD="$LD" \
		NM="$NM" RANLIB="$RANLIB" STRIP="$STRIP" \
		$MAKE_OPTS $@
}

use() {
	/*
	 * Very important function that returns true if a use flag is used, and
	 * false if it is not.
	 */
	local return=1
	for flag in ${USE[@]}; do
		if [[ "$flag" = "$1" ]] || [[ "$flag" = "+$1" ]]; then
			return=0
		elif [[ "$flag" = "-$1" ]]; then
			return=1
		fi
	done
	return $return
}

use_enable() {
	/*
	 * For use with autotools. $(use_enable feature) will return
	 * --enable-feature if the “feature” use flag is set, or
	 * --disable-feature if it is not.
	 * 
	 * Note: $2 can give the name of the feature of the configure
	 *       script, if it is not the same as the use flag.
	 */
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
	/* 
	 * Clone of use_enable(). See the function just before.
	 */
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

ask_use() {
	/*
	 * Function allowing to change a use flag in interactive mode.
	 */
	local ANSWER=
	local use="$1"
	local desc="$(get_use_desc "$use")"
	while [[ ! "$ANSWER" =~ (YES|yes|Y|y|NO|no|N|n) ]]; do
		ask ANSWER "[$name] Do you want to enable use flag \`$use'? [y/n/?]"
			case $ANSWER in
			YES|yes|Y|y)
				USE=(${USE[@]} +$use)
			;;
			NO|no|N|n)
				USE=(${USE[@]} -$use)
			;;
			\?)
				if [[ -n "$desc" ]]; then
					info "$use - $desc"
				else
					info "There is no available information about use flag \`$1'"
				fi
			;;
			*)
				error "Please, answer by \`yes', \`y', \`no' or \`n'."
			;;
		esac
	done
}

uses() {
	if [[ "$PKGMK_INTERACTIVE" = "yes" ]]; then
		for flag in $@; do
			ask_use $flag
		done
	fi
}

make_desktop_entry() {
	/* 
	 * Function which helps to create .desktop files, usually in 
	 * $prefix/applications/. See $menudir.
	 * 
	 * It needs at least one parameter to work, which is the executable name
	 * to be summoned. A second parameter is recommended, to have a complete 
	 * desktop entry, which is the list of the categories the executable is 
	 * in. (this list must be FreeDesktop compliant…)
	 */
	if [[ -z "$1" ]]; then
		error "make_desktop_entry() needs a correct executable name."
		error " | Without a correct exec name, the desktop entry can not be created."
		return 1
	fi
	if [[ -z "$2" ]]; then
		warning "No correct type given for the desktop entry creation."
	fi
	
	local desktop_exec=${1}
	local desktop_type=${2}
	local desktop_name=${3:-${name}}
	local desktop_icon=${4:-${name}}
	local desktop_file=$PKG${menudir}/${5:-${desktop_name}}.desktop
	
	(
	echo "[Desktop Entry]"
	echo "Name=${desktop_name}"
	echo "Type=Application"
	echo "Comment=${description}"
	echo "Exec=${desktop_exec}"
	echo "TryExec=${desktop_exec%% *}"
	echo "Icon=${desktop_icon}"
	echo "Categories=${desktop_type}"
	) > "${desktop_file}"
}

target_arch () {
	/* 
	 * This function returns the architecture depending on the triplet.
	 * The triplet may be the CHOST or the CTARGET, thus allowing 
	 * cross-compilation.
	 *
	 * If CTARGET nor CHOST are declared, the function does not return anything.
	 */
	local TRIPLET
	if [[ -n "$CTARGET" ]]; then
		TRIPLET=$CTARGET
	elif [[ -n "$CHOST" ]]; then
		TRIPLET=$CHOST
	else
		return 0
	fi
	echo $TRIPLET | cut -d- -f 1
}

target_kernel () {
	/* 
	 * Same as target_arch(), but for the OSTYPE.
	 */
	local TRIPLET
	if [[ -n "$CTARGET" ]]; then
		TRIPLET=$CTARGET
	elif [[ -n "$CHOST" ]]; then
		TRIPLET=$CTARGET
	else
		return 0
	fi
	echo $TRIPLET | cut -d- -f 3
}

target_libc () {
	/* 
	 * Same as target_arch() and target_kernel(), but for the last part
	 * of the OSTYPE, which is not always present. If it is not present, 
	 * it returns the third part of the triplet, which is often 
	 * representative of the desired information. The “libc” and the 
	 * kernel are often the same on *BSD systems, which have a *BSD 
	 * kernel and a *BSD libc, for example. It is the same for many other
	 * OSes.
	 */
	local TRIPLET
	local LIBC
	if [[ -n "$CTARGET" ]]; then
		TRIPLET=$CTARGET
	elif [[ -n "$CHOST" ]]; then
		TRIPLET=$CTARGET
	else
		return 0
	fi
	LIBC=$(echo $TRIPLET | cut -d- -f 4)
	if [[ -z "$LIBC" ]]; then
		LIBC=$(echo $TRIPLET | cut -d- -f 3)
	fi
	echo $LIBC
}

pm_arch () {
	/* 
	 * This function returns a correct architecture for the current
	 * package manager, using the triplet_arch() function.
	 * 
	 * Warning : This function is very uncomplete for now. It will be
	 *           completed over time, and will not be really usable 
	 *           before the next stable release.
	 * 
	 * FIXME : Check for kernels, and make corresponding architectures
	 *         for the package managers who works only with archs and 
	 *         not with kernels, etc.
	 */
	/*
	 * No-arch packages must be of the form -noarch, instead of being of 
	 * the form -x86 or whatever.
	 * Note: pacman-g2 doesn’t support — for now — noarch packages, but 
	 *       pacman does.
	 */
	if [[ "$PKGMK_PACKAGE_MANAGER" =~ (pacman|dpkg|opkg|rpm|pkgtools|nhopkg) ]] \
	&& (has no-arch ${archs[@]} || has no-kernel ${kernels[@]}); then
			echo "noarch"
			return 0
	fi
	local TARGET_ARCH=$(target_arch) TARGET_KERNEL=$(target_kernel)
	case ${TARGET_ARCH} in
		parisc*) TARGET_ARCH=hppa ;;
		"Power Macintosh") TARGET_ARCH=ppc ;;
	esac
	case $PKGMK_PACKAGE_MANAGER in
		pacman|pacman-g2|rpm|nhopkg)
			case ${TARGET_ARCH} in
				i?86) TARGET_ARCH=i686 ;;
			esac
		;;
		dpkg)
			case ${TARGET_ARCH} in
				i?86) TARGET_ARCH=i386 ;;
				x86_64) TARGET_ARCH=amd64 ;;
				arm) TARGET_ARCH=armel ;;
				ppc) TARGET_ARCH=powerpc ;;
				powerpc64) TARGET_ARCH=ppc64 ;;
			esac
			case ${TARGET_KERNEL} in
				freebsd*) TARGET_ARCH=kfreebsd-$TARGET_ARCH ;;
				/* Not sure for the others… */
			esac
		;;
	esac
	echo ${TARGET_ARCH}
}

pm_kernel () {
	/* 
	 * Same as pm_arch() but for kernels.
	 */
	local TARGET_KERNEL=$(target_kernel)
	case ${TARGET_KERNEL} in
		freebsd*) TARGET_KERNEL=FreeBSD ;;
		openbsd*) TARGET_KERNEL=OpenBSD ;;
		netbsd*) TARGET_KERNEL=NetBSD ;;
		linux) TARGET_KERNEL=Linux ;;
		/* Feel free to submit patches to complete this… */
	esac
	echo ${TARGET_KERNEL}
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
