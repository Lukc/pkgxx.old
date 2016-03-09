compress_manpages() {
	# compress_manpages() compresses man pages with the tool specified in
	# $PKGMK_MAN_COMPRESSION. On some systems, only "gz" will be supported.
	local FILE DIR TARGET
	
	cd $PKG
	
	# FIXME: This is a temporary solution. Some `man` implementations
	#        can support any kind of compression, but while pkg++
	#        does not support compression/decompression modules
	#        it is no use to try to add things in here. A global
	#        `manpages compression level` would be stupid with
	#        separate modules.
	: ${PKGMK_MANPAGES_COMPLEVEL:=9}

	# For each man page in the package, we run gzip or bzip2, depending on
	# the user’s configuration.
	find .$mandir
	find .$mandir -type f
	find ".${mandir}" -type f | while read FILE; do
		case "$PKGMK_MAN_COMPRESSION" in
			"gz")
				if [[ "$FILE" = "${FILE%%.gz}" ]]; then
					gzip -"$PKGMK_MANPAGES_COMPLEVEL" "$FILE"
				fi
			;;
			"bz2")
				if [[ "$FILE" = "${FILE%%.bz2}" ]]; then
					bzip2 -"$PKGMK_MANPAGES_COMPLEVEL" "$FILE"
				fi
			;;
			"xz")
				if [[ "$FILE" = "${FILE%%.xz}" ]]; then
					xz -"$PKGMK_MANPAGES_COMPLEVEL" "$FILE"
				fi
			;;
		esac
	done
	
	# And now we link compressed manual pages to what should be their 
	# name, whithout compression.
	# Note: We can use $PKGMK_MAN_COMPRESSION because it is already of the
	# form of the compressed files extension.
	find . -type l -path "man/man*#" -o -type l -path "*/man/*/man*/*" | while read FILE; do
		TARGET=`readlink -n "$FILE"`
		TARGET="${TARGET%%.$PKGMK_MAN_COMPRESSION}.$PKGMK_MAN_COMPRESSION"
		rm -f "$FILE"
		FILE="${FILE%%.$PKGMK_MAN_COMPRESSION}.$PKGMK_MAN_COMPRESSION"
		DIR=`dirname "$FILE"`
		
		if [[ -e "$DIR/$TARGET" ]]; then
			ln -sf "$TARGET" "$FILE"
		fi
	done
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
