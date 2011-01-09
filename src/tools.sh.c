
#define make_var(__VAR,__VAL) \
	${__VAR:+__VAR=__VAL}

pkgmake() {
	: ${MAKE=$(path make)}
	: ${MAKE:=$(path gmake)}
	: ${MAKE:=$(path pmake)}
	if [[ -n "$MAKE" ]]; then
		$MAKE \
			make_var(CC,$CC) \
			make_var(CXX,$CXX) \
			make_var(CPP,$CPP) \
			make_var(AS,$AS) \
			make_var(AR,$AR) \
			make_var(LD,$LD) \
			make_var(NM,$NM) \
			make_var(RANLIB,$RANLIB) \
			make_var(STRIP,$STRIP) \
			$MAKE_OPTS $@
	else
		die "No make implementation available."
	fi
}

include () {
	for DIR in $PKGMK_ROOT/../includes $PKGMK_ROOT/../../includes ${PKGMK_INCLUDES_DIR}; do
		if [[ -e $DIR/$1 ]]; then
			if . $DIR/$1; then
				return 0
			else
				return 1 /* Parsing failed. */
			fi
		fi
	done
	return 2 /* Not found. */
}

path() {
	/* 
	 * FIXME: rename to which ?
	 */
	local TARGET="$1"
	/* Damned CPP, no ${PATH//:/ } here. :( */
	for path in $(echo ${PATH} | sed -e "s|:| |g"); do
		if [[ "$TARGET" = "$path" ]]; then
			echo "$path"
			return 0
		fi
	done
	echo "$TARGET"
	return 1
}

wcat() {
	/*
	 * Download and print a file to stdout.
	 */
	local i
	for i in $@; do
		if [[ -n "$(type -p $PKGMK_DOWNLOAD_TOOL)" ]]; then
			$PKGMK_DOWNLOAD_TOOL:cat $i
		else
			curl:cat $i
		fi
	done
}

die() {
	/* 
	 * Display a given error message, and if in debug mode, a traceback.
	 */
	/* $word is used for the transition… */
	local return=$? function file line_number type word
	error "$@"
	if [[ "$PKGMK_DEBUG" = yes ]]; then
		if [[ -n "$return" ]]; then
			echo "${BASH_SOURCE[1]}:${BASH_LINENO[1]}: ${FUNCNAME[1]} returned $return"
		fi
		echo "stack traceback:"
		for ((i = 1; i < ${#FUNCNAME[@]}; i++)) ; do
			function=${FUNCNAME[$i]}
			file=$(basename ${BASH_SOURCE[$i]})
			line_number=${BASH_LINENO[$(($i - 1))]}
			type=$(type -t $function)
			if [[ "$type" =~ function ]]; then
				word="in"
			else
				word="from"
			fi
			echo "	$file:$line_number $word $type '$function'" >&2
		done
	fi
	exit 1
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

pkgsplit() {
	local split="$1"
	shift 1
	cd $PKG
	
	/* 
	 * We use tar to copy a complete tree.
	 */
	for FILE in $@; do
		if [[ -e ".$FILE" ]]; then
			tar c ".$FILE" | (cd $SPLITS/$split ; tar x)
			rm -rf $FILE
		fi
	done
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
	 * The triplet may be the CHOST or the MACHTYPE, thus allowing 
	 * cross-compilation.
	 */
	local TRIPLET
	if [[ -n "$CHOST" ]]; then
		TRIPLET=$CHOST
	else
		TRIPLET=$MACHTYPE /* We use the triplet bash was built with… */
	fi
	echo $TRIPLET | cut -d- -f 1
}

target_kernel () {
	/* 
	 * Same as target_arch(), but for the OSTYPE.
	 */
	local TRIPLET
	if [[ -n "$CHOST" ]]; then
		TRIPLET=$CTARGET
	else
		TRIPLET=$MACHTYPE
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
	if [[ -n "$CHOST" ]]; then
		TRIPLET=$CTARGET
	else
		TRIPLET=$MACHTYPE
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
	 * No-arch packages must be of the form -noarch, instead of being of 
	 * the form -x86 or whatever.
	 */
	local TARGET_ARCH=$(target_arch) TARGET_KERNEL=$(target_kernel)
	if has "$PKGMK_PACKAGE_MANAGER" ${PKGMK_PM_NOARCH_SUPPORT[@]} \
	&& (has no-arch ${archs[@]} || has no-kernel ${kernels[@]}); then
		TARGET_ARCH=noarch ;
	fi
	case ${TARGET_ARCH} in
		parisc*) TARGET_ARCH=hppa ;;
		"Power Macintosh") TARGET_ARCH=ppc ;;
	esac
	if [[ -n "$(type -p $PKGMK_PACKAGE_MANAGER:arch)" ]]; then
		ARCH="${TARGET_ARCH}" KERNEL="${TARGET_KERNEL}" \
			$PKGMK_PACKAGE_MANAGER:arch
	else
		echo "${TARGET_ARCH}" 
	fi
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

#undef make_var

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
