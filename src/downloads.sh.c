
/* 
 * Very unclean, but works.
 */
#define __SRC_UPTODATE info "'$1' already up to date." ; exit 0

download_file() {
	info "Getting '$1'."
	
	PROTOCOL="`get_protocol $1`"
	case $PROTOCOL in
		"http"|"https"|"ftp")
			ftp:download "$1"
		;;
		"file")
			file:copy "$1"
		;;
		*)
			check_command "$PROTOCOL"
			cd $PKGMK_SOURCE_DIR
			if [[ ! -e "$name" ]]; then
				if [[ "$(type ${PROTOCOL}:clone)" != "none" ]]; then
					${PROTOCOL}:clone "$1"
				else
					error "pkg++ has no way to fetch sources via '${PROTOCOL}'."
					exit E_DOWNLOAD
				fi
			elif [[ ! -d "$name" ]]; then
				die "'$PKGMK_SOURCE_DIR/$name' already exists and is not a directory."
			elif [[ ! -r "$name" ]]; then
				die "'$PKGMK_SOURCE_DIR/$name' is not readable."
			else
				/* We don’t check if it is writable, the dl tool will do its job */
				cd $name || die "Couldn't move to '$PKGMK_SOURCE_DIR/$name'."
				if [[ "$(type ${PROTOCOL}:pull)" != "none" ]]; then
					if ! $PROTOCOL:pull "$1" && [[ "$PKGMK_FORCE" != "yes" ]]; then
						__SRC_UPTODATE
					fi
				else
					/* We move and re-clone… if possible. */
					mv "$name" "$name.old"
					download_file "$1"
				fi
				if [[ "$(type ${PROTOCOL}:revision)" != none ]]; then
					export PKGMK_REVISION="$(${PROTOCOL}:revision "$PKGMK_SOURCE_DIR/$name")"
				fi
			fi
		;;
	esac
}

download_source() {
	local FILE LOCAL_FILENAME
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		if [[ ! -e $LOCAL_FILENAME ]] || [[ "$version" = "devel" ]]; then
			if [[ "$LOCAL_FILENAME" = "$FILE" ]]; then
				error "Source file '$LOCAL_FILENAME' not found (can not be downloaded, URL not specified)."
				exit E_DOWNLOAD
			else
				if [[ "$PKGMK_DOWNLOAD" = "yes" ]] || [[ "$version" = "devel" ]]; then
					download_file $FILE
				else
					error "Source file '$LOCAL_FILENAME' not found (use option -d to download)."
					exit E_DOWNLOAD
				fi
			fi
		fi
	done
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
