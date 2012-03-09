
#define make_var(__VAR,__VAL) \
	${__VAR:+__VAR=__VAL}

source () {
	local FILE="$1"
	shift 1
	if [[ "$(dirname "$FILE")" = "." ]]; then
		. "./$FILE" $@
	else
		. "$FILE" $@
	fi
}

type() {
	builtin type -w "$1" | sed -e "s/${1//\//\\/\\/}: //"
}

tac() {
	if [[ -n "$tac" ]]; then
		"$tac" $@
	else
		nl -ba $@ | sort -nr | cut -f2-
	fi
}

pkgmake() {
	/* FIXME: DEPRECATION */
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
	if [[ -n "$make" ]]; then
		$make \
			make_var(CC,$CC) \
			make_var(CXX,$CXX) \
			make_var(CPP,$CPP) \
			make_var(AS,$AS) \
			make_var(AR,$AR) \
			make_var(LD,$LD) \
			make_var(NM,$NM) \
			make_var(RANLIB,$RANLIB) \
			make_var(STRIP,$STRIP) \
			${make_opts[@]:-$MAKE_OPTS} $@
	else
		die "No make implementation available."
	fi
}

include() {
	for FILE in $@; do
		for DIR in $PKGMK_ROOT $PKGMK_ROOT/../includes $PKGMK_ROOT/../../includes ${PKGMK_INCLUDES_DIR}; do
			if [[ -e $DIR/$FILE ]]; then
				if . $DIR/$FILE; then
					continue 2
				else
					error "An error occured while including \`$DIR/$FILE'."
					return 1 /* Parsing failed. */
				fi
			fi
		done
		error "Include \`$FILE' was not found."
		return 2 /* Not found. */
	done
}

sedi() {
	/* 
	 * Emulates the sed -i feature.
	 * Usage: sedi sed-script files
	 * Note: Stollen from c4o. (Cruxports for OpenBSD)
	 */
	TMP1=$(mktemp sedi.XXXXXXXXXX) || die "mktemp [sedi] failed."
	REGEX="$1"
	shift
	if istrue $sed_gnu; then
		$sed "$REGEX" -i $@
	else
		/* 
		 * Useless to try to use -isuffix, it could destroy a file 
		 * that already exists, even if it is very few probable, or 
		 * would need more code…
		 */
		for i in $*; do
			sed "$REGEX" "$i" > $TMP1 || die "sed '$REGEX' '$i' [sedi] or redirection to '$TMP1' failed."
			/* Preserve permissions, owner, etc. */
			cat $TMP1 > "$i" || die "cat '$TMP1' [sedi] or redirection to '$i' failed."
		done
	fi
	rm $TMP1
	unset TMP1
}

replace() {
	/* 
	 * Replaces pattern $1 by pattern $2 in file $3.
	 */
	sedi "s|${1//|/\|}|${2//|/\|}|" "$3"
}

which() {
	local TARGET="$1"
	/* Damned CPP, no ${PATH//:/ } here. :( */
	for path in $(echo ${PATH} | sed -e "s|:| |g"); do
		if [[ -x "$path/$TARGET" ]]; then
			echo "$path/$TARGET"
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
		if [[ "$(type ${PKGMK_DOWNLOAD_TOOL}:cat)" != none ]]; then
			${PKGMK_DOWNLOAD_TOOL}:cat $i
		else
			curl:cat $i
		fi
	done
}

die() {
	/* 
	 * Display a given error message.
	 */
	/* $word is used for the transition… */
	error "${?:+!$?!} $@"
	exit 1
}

use() {
	/*
	 * Very important function that returns true if a use flag is used, and
	 * false if it is not.
	 */
	local return=2
	for flag in ${USE[@]}; do
		if [[ "$flag" = "$1" ]] || [[ "$flag" = "+$1" ]]; then
			return=0
		elif [[ "$flag" = "-$1" ]]; then
			return=1
		fi
	done
	if (( $return == 2 )); then
		local eval=$(eval "echo \${use_${1}[1]}")
		if [[ "$eval" = 1 ]]; then
			eval=0
		elif [[ "$eval" = 0 ]]; then
			/* Do not use because not specified otherwise anywhere */
			eval=3
		fi
		if [[ -n "$eval" ]]; then
			return $eval
		fi
	fi
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

target_arch () {
	/* 
	 * This function return the architecture depending on the triplet.
	 * The triplet may be the CHOST or the MACHTYPE, thus allowing 
	 * cross-compilation.
	 */
	if [[ -n "$CHOST" ]]; then
		echo $CHOST | cut -d- -f 1
	else
		echo $MACHTYPE /* We use the architecture zsh was built with… */
	fi
}

target_kernel () {
	/* 
	 * Same as target_arch(), but for the OSTYPE.
	 */
	if [[ -n "$CHOST" ]]; then
		echo $CHOST | cut -d- -f 3
	else
		echo $OSTYPE | cut -d- -f 1
	fi
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
		TRIPLET=$CHOST
	else
		TRIPLET=1-2-$OSTYPE
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
	if [[ "$(type ${PKGMK_PACKAGE_MANAGER}:arch)" != none ]]; then
		ARCH="${TARGET_ARCH}" KERNEL="${TARGET_KERNEL}" \
			${PKGMK_PACKAGE_MANAGER}:arch
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

vercmp () {
	local comp=$2
	local i=1
	local version1=$(echo "$1" | sed -e "s/-/./g;s/rc\([0-9]\)/.rc.\1/g;s/\.\././g").0
	local version2=$(echo "$3" | sed -e "s/-/./;s/rc\([0-9]\)/.rc.\1/g;s/\.\././g")
	
	/* 
	 * Traditional sh comparison operators should be supported, because
	 * they don’t require to be quoted
	 */
	case "$comp" in
		-gt)
			comp=">"
		;;
		-ge)
			comp=">="
		;;
		-lt)
			comp="<"
		;;
		-le)
			comp="<="
		;;
		-eq)
			comp="=="
		;;
		-ne)
			comp="!="
		;;
		-sb)
			/* 
			 * Standing for Same Branch.
			 */
			comp="~>"
		;;
	esac
	
	if [[ "$comp" != "~>" ]]; then
		version2="$version2".0
	fi
	local v1=$(echo "$version1" | cut -d '.' -f $i)
	local v2=$(echo "$version2" | cut -d '.' -f $i)
	
	alpha=-3
	beta=-2
	rc=-1
	devel=99999 /* If you ever find a version greater than this, update */
	while [[ -n "$v1" && -n "$v2" ]]; do
		if ! (( $v1 == $v2 )); then
			if [[ "$comp" =~ ("!="|"~=") ]]; then
				return 0
			elif [[ "$comp" == "~>" ]]; then
				return 1
			else
				(( $v1 $comp $v2 ))
				return $?
			fi
		fi
		i=$(($i+1))
		v1=$(echo "$version1" | cut -d "." -f $i)
		v2=$(echo "$version2" | cut -d "." -f $i)
		[[ -z "$v2" && "$comp" == "~>" ]] && return 0
		[[ -n "$v1" && -z "$v2" ]] && v2=0
		[[ -n "$v2" && -z "$v1" ]] && v1=0
	done
	/* If everything was equal. */
	[[ "$comp" =~ ("!="|"~="|">"|"<") ]] && return 1 || return 0
}

@{ () {
	case $# in
		0|1|2)
			error "@{ needs 3 parameters and to be closed."
			return 1
		;;
		3)
			error "@{ needs to be closed."
			return 1
		;;
		4);;
		*)
			error "@{ needs only 3 parameters and to be closed."
			return 1
		;;
	esac
	/* This is debug to avoid having problems with vercmp. */
	echo "$1$2$3" | (egrep -q "[ 	]") && {
			error "@{: opts must not contain any space or tabulation."
			return 1
		}
	for i in 1 2 3; do
		eval "[[ -z \$$i ]]" && {
			error "@{: empty opt (\$$i)."
			return 1
		}
	done
	vercmp "$1" "$2" "$3"
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
			sed -e "s/.*${tarname}-//;s/\.tar\.\(gz\|bz2\|xz\|lzma\|lzo\).*//"
	)
}

depname() {
	/* 
	 * What could be worse than finding a "gcc >= 4.6" in a… well, in 
	 * something not able to manage dependencies. To avoid the awful 
	 * previous situation, we give depname() to module maintainer, 
	 * allowing them to get the name of their dependencies without
	 * doing the job themselves (which we can’t call “maintaining”).
	 * 
	 * REMEMBER TO UPDATE THIS IF AND WHEN YOU CHANGE SOMETHING IN
	 * THE depends[] SYNTAX.
	 */
	echo "$1" | sed -e "s/ .*//;s/(>|<|>=|<=|=|==|~>|~<|!=|~=).*//"
}

#undef make_var

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
