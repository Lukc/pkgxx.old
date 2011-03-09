
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
	tar ${TARFLAGS}c $@ > $TARBALL
}

tar:list() {
	/* 
	 * FIXME: It’s not an “untar tool”, here, but a “tar listing tool”…
	 */
	case "$PKGMK_UNTAR_TOOL" in
		gtar)
			tar tvvf "$1" | \
				sed 's|  *|	|g' | \
				cut -d "	" -f 1,2,6,7,8
		;;
		bsdtar)
			bsdtar tvf "$1" | \
				sed 's|  *|	|g' | \
				cut -d "	" -f 1,3,4,9,10,11,12
		;;
		sltar)
			cat "$1" | sltar t /* Hum… we should be able to do a little better… a day. */
		;;
		*)
			if [[ -n "$(type -t $PKGMK_UNTAR_TOOL:list)" ]]; then
				$PKGMK_UNTAR_TOOL:list "$@"
			else
				die "Not supported tar implementation."
			fi
		;;
	esac
}

unpack_source() {
	/*
	 * unpack_source() unpacks any file in ${source[ ]}.
	 */
	local FILE LOCAL_FILENAME COMMAND
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		/*
		 * $no_extraction can be set to true or yes in Pkgfiles. If so,
		 * pkg++ doesn’t extract any archive nor uncompress any file.
		 */
		if ! [[ "$no_extraction" =~ ("true"|"yes") ]]; then
			case $LOCAL_FILENAME in
				*.tar|*.tar.gz|*.tar.Z|*.tgz|*.tar.bz2|*.tbz2|*.tar.xz|*.txz|*.tar.lzma)
					$PKGMK_UNTAR_TOOL:unpack "$LOCAL_FILENAME" "$SRC"
				;;
				*)
					local LOCAL_FILETYPE="$(basename "$LOCAL_FILENAME:unpack")"
					if [[ -n "$(type -p $LOCAL_EXTENSION)" ]]; then
						if ! $LOCAL_EXTENSION:unpack; then
							error "Unpacking of '$FILE' failed."
						fi
					else
						info "Copying $LOCAL_FILENAME."
						cp -r "$LOCAL_FILENAME" "$SRC"
					fi
				;;
			esac
		fi
		
		/*
		 * If we failed… we abord everything and remove the work dir.
		 */
		if [[ $? != 0 ]]; then
			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi
			error "Building '$TARGET' failed."
			exit E_UNPACK
		fi
	done
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
