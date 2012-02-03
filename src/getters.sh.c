
get_protocol() {
	/*
	 * It just returns what is before the first “:” and after the first “+”.
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
		[[ $1 =~ ^[aA-zZ]:\/\/.* ]] || \
		[[ $1 =~ ^[aA-zZ]\+.*:\/\/.* ]]
	then
		local NORMAL_RETURN="$PKGMK_SOURCE_DIR/${match[2]}"
		local PROTOCOL="`get_protocol "$1"`"
		case $PROTOCOL in
			file)
				echo -n "$PKGMK_SOURCE_DIR/"
				echo "$1" | sed -e "s|file://||"
			;;
			*)
				if [[ "$(type ${PROTOCOL}:clone)" != "none" ]] ; then
					/* A VCS download URL */
					echo "$PKGMK_SOURCE_DIR/$name"
				else
					/* Well, something else. */
					echo "$NORMAL_RETURN"
				fi
			;;
		esac
	else
		echo $1
	fi
}

get_basename() {
	/*
	 * Returns the base name of a file or URL.
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
	/* 
	 * Prints on stdout the description of a use flag. The description can
	 * be defined anywhere in the configuration, recipe or include.
	 *
	 * Each use flag can have a description. Use `use_flag[2]="desc"` to
	 * define a description for `flag`. Redefining use descriptions is 
	 * however something to avoid.
	 */
	local use="$1"
	eval "echo \${use_${use}[2]}"
}

get_target() {
	/* 
	 * Prints on stdout the absolute name and path of a package, making
	 * the required calls to the required parts of the wanted package
	 * manager’s module.
	 */
	${PKGMK_PACKAGE_MANAGER}:target
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
