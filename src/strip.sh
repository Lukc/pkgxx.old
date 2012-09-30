strip_files() {
	local FILE FILTER
	
	cd $PKG
	
	if [[ -f $PKGMK_ROOT/$PKGMK_NOSTRIP ]]; then
		FILTER="grep -v -f $PKGMK_ROOT/$PKGMK_NOSTRIP"
	else
		FILTER="cat"
	fi
	
	(
		if istrue $find_gnu; then
			find . -type f -printf "%P\n"
		else
			find . -type f | sed "s|\./||"
		fi
	) \
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

# vim:set syntax=sh shiftwidth=4 tabstop=4:
