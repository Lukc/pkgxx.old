
get_protocol() {
	/*
	 * It just returns what is before the first “:”.
	 */
	local PROTOCOL="`echo $1 | cut -d ':' -f 1`"
	echo ${PROTOCOL/\+*}
}
get_filename() {
	/*
	 * Returns the name of the file/directory, even in an URL.
	 */
	if
		[[ $1 =~ ^(http|https|ftp):\/\/.*/(.+) ]] || \
		[[ $1 =~ ^file:\/\/.* ]] || \
		[[ $1 =~ ^(svn|git|hg|mtn|bzr):\/\/.* ]] || \
		[[ $1 =~ ^(svn|git|hg|mtn|bzr)\+.*:\/\/.* ]]
	then
		local NORMAL_RETURN="$PKGMK_SOURCE_DIR/${match[2]}"
		local PROTOCOL="`get_protocol "$1"`"
		case $PROTOCOL in
			svn|git|hg|mtn|bzr)
				echo "$PKGMK_SOURCE_DIR/$name"
			;;
			file)
				echo -n "$PKGMK_SOURCE_DIR/"
				echo "$1" | sed -e "s|file://||"
			;;
			*)
				echo "$NORMAL_RETURN"
			;;
		esac
	else
		echo $1
	fi
}

get_basename() {
	/*
	 * Returns something. Probably the complete path of a file on a server.
	 */
	local FILE="`echo $1 | sed 's|^.*://.*/||g'`"
	echo $FILE
}

get_pkgfile() {
	/*
	 * Returns the name of the Pkgfile.
	 */
	setopt -G
	local PKGFILE=
	if [[ -n "$PKGMK_PKGFILE" ]]; then
		echo "$PKGMK_PKGFILE"
	elif [[ -n $(echo -n $PKGMK_PKGFILE_NAME-*) ]]; then
		for file in $PKGMK_PKGFILE_NAME-*; do
			if [[ "$file" =~ $PKGMK_PKGFILE_NAME-[0-9] ]]; then
				PKGFILE=$file
			fi
		done
		echo "$PKGFILE"
	else
		echo "$PKGMK_PKGFILE_NAME"
	fi
}

get_metafile() {
	/*
	 * Returns the name of any metafile.
	 */
	local VERSION="`basename "$PKGMK_PKGFILE" | sed -e "s|$PKGMK_PKGFILE_NAME-||"`"
	local DIR="`dirname "$PKGMK_PKGFILE"`"
	if [[ "$VERSION" = `basename "$PKGMK_PKGFILE"` ]]; then
		echo "$DIR/$1"
	else
		echo "$DIR/$1-$VERSION"
	fi
}

get_use_description() {
	/* FIXME-0.12: Depreciation. */
	local use="$1"
	local FILE
	local desc=$((
		if [[ -e _SHAREDIR/pkg++/uses ]]; then
			egrep "^${use}\|" _SHAREDIR/pkg++/uses
		fi
		if [[ -n `ls _SYSCONFDIR/pkg++/uses/` ]]; then
			for FILE in _SYSCONFDIR/pkg++/uses/\*; do
				egrep "^${use}\|" $FILE
			done
		fi
	) | tail -n 1 \
	  | cut -d "|" -f 2)
	if [[ -n "$desc" ]]; then
		error
		error "The old use flags description format is deprecated."
		error "It's support will be removed for pkg++ 0.12"
		error
	fi
	if [[ -z "$desc" ]]; then
		desc="$(eval "echo \${use_${use}[2]}")" # 1 is default status
	fi
	echo "$desc"
}

get_target() {
	${PKGMK_PACKAGE_MANAGER}:target
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
