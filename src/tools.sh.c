
has() {
	/*
	 * If $1 is in the other parameters, then returns true. Else returns 
	 * false.
	 */
	item="$1"
	shift 1
	for t_item in $@; do
		if [[ "$item" = "$t_item" ]]; then
			return 0
		fi
	done
	return 1
}

pkgmake() {
	/*
	 * That avoids to use a non-gnu make, which could cause problems on 
	 * some ports.
	 */
	GMAKE $MAKE_OPTS $@
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
	local feature=${2:=$flag}
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
	local feature=${2:=$flag}
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
	
	cat <<-EOF > "${desktop_file}"
	[Desktop Entry]
	Name=${desktop_name}
	Type=Application
	Comment=${description}
	Exec=${desktop_exec}
	TryExec=${desktop_exec%% *}
	Icon=${desktop_icon}
	Categories=${desktop_type}
	EOF
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
	local TARGET_ARCH=$(target_arch)
	case ${TARGET_ARCH} in
		parisc*) arch=hppa ;;
		"Power Macintosh") arch=ppc ;;
	esac
	case $PKGMK_PACKAGE_MANAGER in
		pacman|pacman-g2|rpm|nhopkg)
			case ${TARGET_ARCH} in
				i?86) arch=i686 ;;
			esac
		;;
		dpkg)
			case ${TARGET_ARCH} in
				i?86) arch=i386 ;;
				x86_64) arch=amd64 ;;
				arm) arch=armel ;;
				ppc) arch=powerpc ;;
				powerpc64) arch=ppc64 ;;
			esac
		;;
	esac
	echo ${TARGET_ARCH}
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
