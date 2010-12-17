
ftp:download() {
	
	check_command "$DOWNLOAD_TOOL"
	
	LOCAL_FILENAME=`get_filename $1`
	LOCAL_FILENAME_PARTIAL="$LOCAL_FILENAME.partial"
	
	if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
		DOWNLOAD_OPTS="-f -L --insecure"
	elif [[ "$DOWNLOAD_TOOL" = "axel" ]]; then
		DOWNLOAD_OPTS="-o $LOCAL_FILENAME_PARTIAL -a"
	elif [[ "$DOWNLOAD_TOOL" = "wget" ]]; then
		DOWNLOAD_OPTS="--passive-ftp --no-directories --tries=3 --waitretry=3 \
			--directory-prefix=$PKGMK_SOURCE_DIR \
			--output-document=$LOCAL_FILENAME_PARTIAL --no-check-certificate"
	fi
	
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		info "Partial download found, trying to resume"
		if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
			RESUME_CMD="-C $LOCAL_FILENAME_PARTIAL"
		elif [[ "$DOWNLOAD_TOOL" = "axel" ]]; then
			RESUME_CMD="" /* FIXME */
		elif [[ "$DOWNLOAD_TOOL" = "wget" ]]; then
			RESUME_CMD="-c"
		fi
	fi
	
	error=1
	
	BASENAME=`get_basename $1`
	for REPO in ${PKGMK_SOURCE_MIRRORS[@]}; do
		REPO="`echo $REPO | sed 's|/$||'`"
		if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
			curl $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_CURL_OPTS $REPO/$BASENAME > $LOCAL_FILENAME_PARTIAL
		elif [[ "$DOWNLOAD_TOOL" = "axel" ]]; then
			axel $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_AXEL_OPTS $REPO/$BASENAME
		elif [[ "$DOWNLOAD_TOOL" = "wget" ]]; then
			wget $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_WGET_OPTS $REPO/$BASENAME
		fi
		error=$?
		if [[ $error == 0 ]]; then
			break
		fi
	done
	
	if [[ $error != 0 ]]; then
		while true; do
			if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
				curl $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_CURL_OPTS $1 > $LOCAL_FILENAME_PARTIAL
			elif [[ "$DOWNLOAD_TOOL" = "axel" ]]; then
				axel $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_WGET_OPTS $1
			elif [[ "$DOWNLOAD_TOOL" = "wget" ]]; then
				wget $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_WGET_OPTS $1
			fi
			error=$?
			if [[ $error != 0 ]] && [[ "$RESUME_CMD" ]]; then
				info "Partial download failed, restarting"
				rm -f "$LOCAL_FILENAME_PARTIAL"
				RESUME_CMD=""
			else
				break
			fi
		done
	fi
		
	if [[ $error != 0 ]]; then
		error "Downloading '$1' failed."
		exit E_DOWNLOAD
	fi
		
	mv -f "$LOCAL_FILENAME_PARTIAL" "$LOCAL_FILENAME"
	
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
