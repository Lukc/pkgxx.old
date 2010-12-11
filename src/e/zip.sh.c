
/* Look at tar.sh.c for more informations about the structure of this file. */

zip:unpack() {
	info "Unpacking $1."
	case "$PKGMK_UNZIP_TOOL" in
		unzip)
			unzip -qq -o -d $2 $1
		;;
		bsdtar)
			bsdtar -p -o -C $2 -xf $1
		;;
	esac
}

zip:pack() {
	/* 
	 * FIXME
	 */
	:
}

zip:list() {
	/* 
	 * FIXME
	 */
	:
}

