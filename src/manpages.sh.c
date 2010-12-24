
compress_manpages() {
	/*
	 * compress_manpages() compresses man pages. I wonder why I write that.
	 */
	local FILE DIR TARGET
	
	cd $PKG
	
	/*
	 * For each man page in the package, we run gzip or bzip2, depending on
	 * the user’s configuration.
	 */
	find . -type f -path "*/man/man*/*" -o -type f -path "*/man/*/man*/*" | while read FILE; do
		case "$PKGMK_MAN_COMPRESSION" in
			"gz")
				if [[ "$FILE" = "${FILE%%.gz}" ]]; then
					gzip -9 "$FILE"
				fi
			;;
			"bz2")
				if [[ "$FILE" = "${FILE%%.bz2}" ]]; then
					bzip2 -9 "$FILE"
				fi
			;;
			"xz")
				if [[ "$FILE" = "${FILE%%.xz}" ]]; then
					xz -9 "$FILE"
				fi
			;;
		esac
	done
	
	/*
	 * And now we link compressed manual pages to what should be their 
	 * name, whithout compression.
	 * Note: We can use $PKGMK_MAN_COMPRESSION because it is already of the
	 * form of the compressed files extension.
	 */
	find . -type l -path "*/man/man*/*" -o -type l -path "*/man/*/man*/*" | while read FILE; do
		TARGET=`readlink -n "$FILE"`
		TARGET="${TARGET##*/}"
		TARGET="${TARGET%%.$PKGMK_MAN_COMPRESSION}.$PKGMK_MAN_COMPRESSION"
		rm -f "$FILE"
		FILE="${FILE%%.$PKGMK_MAN_COMPRESSION}.$PKGMK_MAN_COMPRESSION"
		DIR=`dirname "$FILE"`
		
		if [[ -e "$DIR/$TARGET" ]]; then
			ln -sf "$TARGET" "$FILE"
		fi
	done
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
