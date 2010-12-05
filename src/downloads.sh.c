
#define __SRC_UPTODATE info "'$1' already up to date."

download_file() {
	info "Getting '$1'."
	
	PROTOCOL="`get_protocol $1`"
	case $PROTOCOL in
		"http"|"https"|"ftp"|"file")
			ftp:download "$1"
		;;
		*)
			check_command "$PROTOCOL"
			cd $PKGMK_SOURCE_DIR
			if cd $name 2&> /dev/null; then
				if ! $PROTOCOL:pull "$1"; then
					__SRC_UPTODATE
				fi
			else
				$PROTOCOL:clone "$1"
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
