
/* 
 *  *** READ THIS :
 * tar:unpack() extracts an archive ($1) to a directory ($2).
 * tar:pack() creates an archive ($1) which contains files or directory ($@-$1).
 * tar:list() gives the content of an archive, with only the datas needed to 
 *            make a footprint.
 */

tar:unpack() {
	local COMPRESS
	info "Unpacking $1."
	case "$PKGMK_UNTAR_TOOL" in
		gtar)
			case "$1" in
				*.gz|*.Z|*.tgz) COMPRESS="z" ;;
				*.bz2|*.tbz2) COMPRESS="j" ;;
				*.xz|*.txz) COMPRESS="J" ;;
				*.lzma) COMPRESS=" --lzma -" ;;
			esac
			tar -C $2 -x${COMPRESS}f $1
		;;
		bsdtar)
			bsdtar -p -o -C $2 -xf $1
		;;
		sltar)
			(
				case "$1" in
					*.gz|*.Z|*.tgz)
						gzip -dc "$1"
					;;
					*.bz2|*.tbz2)
						bzip2 -dc "$1"
					;;
					*.xz|*.txz|*.lzma)
						xz -dc "$1"
					;;
				esac
			) | ( cd "$2" ; sltar x )
		;;
	esac
}

tar:pack() {
	[[ "$PKGMK_UNTAR_TOOL" = sltar ]] && die "sltar does not supports packing."
	local TARBALL="$1" TARFLAGS
	shift 1
	case $TARBALL in
		*.xz|*.txz)
			TARFLAGS=J
		;;
		*.bz2|*.tbz2)
			TARFLAGS=j
		;;
		*.gz|*.tgz)
			TARFLAGS=z
		;;
		*.tar) ;;
		*)
			die "Don’t know how to make '$TARBALL'."
		;;
	esac
	if [[ "$PKGMK_DEBUG" = "yes" ]]; then
		TARFLAGS=${TARFLAGS}v
	fi
	tar ${TARFLAGS}c $@ > $TARBALL
}

tar:list() {
	/* 
	 * FIXME
	 */
	:
}

