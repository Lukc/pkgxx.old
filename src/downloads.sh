download_file() {
	info "$msg_getting" "$1"
	
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
					error "$msg_cannot_fetch_through_protocol" "${PROTOCOL}"
					exit $E_DOWNLOAD
				fi
			elif [[ ! -d "$name" ]]; then
				die "$msg_file_is_not_directory" "$PKGMK_SOURCE_DIR/$name"
			elif [[ ! -r "$name" ]]; then
				die "$msg_file_not_readable" "$PKGMK_SOURCE_DIR/$name"
			else
				# We don’t check if it is writable, the dl tool will do its job
				cd $name || die "$msg_could_not_cd" "$PKGMK_SOURCE_DIR/$name"
				if [[ "$(type ${PROTOCOL}:pull)" != "none" ]]; then
					if ! ${PROTOCOL}:pull "$1"; then
						info "$msg_already_u2d" "$1"
					fi
				else
					# We move and re-clone… if possible.
					cd ..
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
				error "$msg_localfile_not_found" "$LOCAL_FILENAME"
				exit $E_DOWNLOAD
			else
				if [[ "$PKGMK_DOWNLOAD" = "yes" ]]; then
					download_file $FILE
				else
					error "$msg_localfile_not_downloaded" "$LOCAL_FILENAME"
					exit $E_DOWNLOAD
				fi
			fi
		fi
	done
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
