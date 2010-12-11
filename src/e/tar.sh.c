
/* 
 *  *** READ THIS :
 * tar:unpack() extracts an archive ($1) to a directory ($2).
 * tar:pack() creates an archive ($1) which contains files or directory ($@-$1).
 * tar:list() gives the content of an archive, with only the datas needed to 
 *            make a footprint.
 */

tar:unpack() {
	local COMPRESS
	case "$PKGMK_UNTAR_TOOL" in
		gtar)
			case "$1" in
				*.gz|*.Z|*.tgz) COMPRESS="z" ;;
				*.bz2|*.tbz2) COMPRESS="j" ;;
				*.xz|*.txz) COMPRESS="J" ;;
				*.lzma) COMPRESS=" --lzma -" ;;
			esac
			info "Unpacking $1."
			tar -C $2 -x${COMPRESS}f $1
		;;
		bsdtar)
			info "Unpacking $1."
			bsdtar -p -o -C $2 -xf $1
		;;
	esac
}

tar:pack() {
	/* 
	 * FIXME
	 */
	:
}

tar:list() {
	/* 
	 * FIXME
	 */
	:
}

