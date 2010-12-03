
strip_files() {
	local FILE FILTER
	
	cd $PKG
	
	if [[ -f $PKGMK_ROOT/$PKGMK_NOSTRIP ]]; then
		FILTER="grep -v -f $PKGMK_ROOT/$PKGMK_NOSTRIP"
	else
		FILTER="cat"
	fi
	
	/*
	 * This corrects a problem on non-GNU userlands…
	 */
	#if defined gfind
	find . -type f -printf "%P\n" \\
	/*
	 * This one uses the GNU find, and it’s -printf option.
	 */
	#else
	find . -type f | sed "s|\./||" \\
	/*
	 * And this one uses sed to truncate the begin of the relative file’s path.
	 */
	#endif
		| $FILTER | while read FILE; do
		case $(file -b "$FILE") in
		*ELF*executable*not\ stripped)
			${STRIP:-strip} --strip-all "$FILE"
			;;
		*ELF*shared\ object*not\ stripped)
			${STRIP:-strip} --strip-unneeded "$FILE"
			;;
		current\ ar\ archive)
			${STRIP:-strip} --strip-debug "$FILE"
		esac
	done
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
