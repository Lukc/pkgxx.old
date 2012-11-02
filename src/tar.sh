# Internal tar module.
PKGMK_UNTAR_TOOLS=(${PKGMK_UNTAR_TOOLS[@]} gtar bsdtar sltar)

gtar:unpack() {
	local COMPRESS
	subinfo "$msg_unpacking" "${1}"
	case "$1" in
		*.gz|*.Z|*.tgz) COMPRESS="z" ;;
		*.bz2|*.tbz2) COMPRESS="j" ;;
		*.xz|*.txz|*.lzma) COMPRESS="J" ;;
	esac
	tar -C $2 -x${COMPRESS}f $1
}

bsdtar:unpack() {
	subinfo "$msg_unpacking" "${1}"
	bsdtar -p -o -C $2 -xf $1
}

sltar:unpack() {
	subinfo "$msg_unpacking" "${1}"
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
	local TARBALL="$1"
	if [[ -z "$TARFLAGS" ]]; then
		local TARFLAGS
	fi
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
			[[ -z "$TARFLAGS" ]] && die "Don’t know how to make '$TARBALL'."
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
			# FIXME: Maybe there’s a way to remove the second sed?
			bsdtar tvf "$1" | \
				sed 's|  *|	|g' | \
				cut -d "	" -f 1,3,4,9,10,11,12 | \
				sed 's|	|/|;s|	|/|;s|/|	|'
		;;
		sltar)
			# FIXME: Use a filter to give enough informations.
			sltar t < "$1"
		;;
		*)
			if [[ -n "$(type "${PKGMK_TARLIST_TOOL}:list" 2>/dev/null)" ]]; then
				${PKGMK_TARLIST_TOOL}:list "$@"
			else
				die "$msg_not_supported_tar_implementation"
			fi
		;;
	esac
}
