/* 
 * Internal tar module.
 */
untar_add(gtar bsdtar sltar)

gtar:unpack() {
	local COMPRESS
	info "Unpacking '$1'."
	case "$1" in
		*.gz|*.Z|*.tgz) COMPRESS="z" ;;
		*.bz2|*.tbz2) COMPRESS="j" ;;
		*.xz|*.txz) COMPRESS="J" ;;
		*.lzma) COMPRESS=" --lzma -" ;;
	esac
	tar -C $2 -x${COMPRESS}f $1
}

bsdtar:unpack() {
	info "Unpacking '$1'."
	bsdtar -p -o -C $2 -xf $1
}

sltar:unpack() {
	info "Unpacking '$1'."
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
}

tar:pack() {
	[[ "$PKGMK_UNTAR_TOOL" = sltar ]] && die "sltar does not support packing."
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
	tar -${TARFLAGS}cf "${TARBALL}" $@
}

tar:list() {
	case "${PKGMK_TARLIST_TOOL:=$PKGMK_UNTAR_TOOL}" in
		gtar)
			tar tvvf "$1" | \
				sed 's|  *|	|g' | \
				cut -d "	" -f 1,2,6,7,8
		;;
		bsdtar)
			/* 
			 * FIXME: Maybe there’s a way to remove the second sed?
			 */
			bsdtar tvf "$1" | \
				sed 's|  *|	|g' | \
				cut -d "	" -f 1,3,4,9,10,11,12 | \
				sed 's|	|/|;s|	|/|;s|/|	|'
		;;
		sltar)
			/* 
			 * FIXME: Use a filter to give enough informations.
			 */
			sltar t < "$1"
		;;
		*)
			if [[ -n "$(type "${PKGMK_TARLIST_TOOL}:list" 2>/dev/null)" ]]; then
				${PKGMK_TARLIST_TOOL}:list "$@"
			else
				die "Not supported tar implementation."
			fi
		;;
	esac
}
