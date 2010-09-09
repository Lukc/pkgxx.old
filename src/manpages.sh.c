
compress_manpages() {
	local FILE DIR TARGET

	cd $PKG
	
	find . -type f -path "*/man/man*/*" | while read FILE; do
		if [[ "$FILE" = "${FILE%%.gz}" ]]; then
			gzip -9 "$FILE"
		fi
	done
	
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

