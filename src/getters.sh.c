
get_protocol() {
	/*
	 * It just returns what is before the first “:”.
	 */
	local PROTOCOL="`echo $1 | cut -d ':' -f 1`"
	echo $PROTOCOL
}
get_filename() {
	/*
	 * Returns the name of the file/directory, even in an URL.
	 */
	if
		[[ $1 =~ ^(http|https|ftp):\/\/.*/(.+) ]] || \
		[[ $1 =~ file:\/\/.* ]] || \
		[[ $1 =~ (svn|git|hg|bzr):.*:\/\/.* ]]
	then
		local NORMAL_RETURN="$PKGMK_SOURCE_DIR/${BASH_REMATCH[2]}"
		local PROTOCOL="`get_protocol $1`"
		case $PROTOCOL in
			svn|git|hg|bzr)
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
	local PKGFILE=
	if [[ -n "$PKGMK_PKGFILE" ]]; then
		echo "$PKGMK_PKGFILE"
	elif [[ -n `ls $PKGMK_PKGFILE_NAME-* 2>/dev/null` ]]; then
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
	if [[ "$VERSION" = `basename $PKGMK_PKGFILE` ]]; then
		echo "$DIR/.$1"
	else
		echo "$DIR/.$1-$VERSION"
	fi
}

get_use_desc() {
	local use="$1"
	local FILE
	(
		if [[ -e _SHAREDIR/pkg++/uses ]]; then
			grep "$use" _SHAREDIR/pkg++/uses
		fi
		if [[ -n `ls _SYSCONFDIR/pkg++/uses/` ]]; then
			for FILE in _SYSCONFDIR/pkg++/uses/\*; do
				grep "$use" $FILE
			done
		fi
	) | tail -n 1 \\
	  | cut -d "|" -f 2
}
/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
