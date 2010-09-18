
compress_manpages() {
	/*
	 * compress_manpages() compresses man pages. I wonder why I write that.
	 */
	local FILE DIR TARGET
	
	cd $PKG
	
	/*
	 * For each man page in the package, we run gzip.
	 */
	find . -type f -path "*/man/man*/*" | while read FILE; do
		if [[ "$FILE" = "${FILE%%.gz}" ]]; then
			gzip -9 "$FILE"
		fi
	done
	
	/*
	 * And now we link gzipped manual pages to what should be their name, 
	 * whithout gzip.
	 */
	find . -type l -path "*/man/man*/*" | while read FILE; do
		TARGET=`readlink -n "$FILE"`
		TARGET="${TARGET##*/}"
		TARGET="${TARGET%%.gz}.gz"
		rm -f "$FILE"
		FILE="${FILE%%.gz}.gz"
		DIR=`dirname "$FILE"`

		if [[ -e "$DIR/$TARGET" ]]; then
			ln -sf "$TARGET" "$FILE"
		fi
	done
}

