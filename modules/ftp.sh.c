
curl:download() {
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		RESUME_CMD="-C $LOCAL_FILENAME_PARTIAL"
	fi
	DOWNLOAD_OPTS="-f -L --insecure"
	curl $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_CURL_OPTS $1 > $LOCAL_FILENAME_PARTIAL
}

curl:cat() {
	curl $1 2>/dev/null
}

axel:download() {
	/* 
	 * axel tries to resume downloads when the output file already 
	 * exists… so don’t need to manage that. ;)
	 */
	DOWNLOAD_OPTS="-o $LOCAL_FILENAME_PARTIAL -a"
	axel $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_AXEL_OPTS $1
}

axel:cat() {
	local tmp="`mktmp`"
	axel --output "$tmp" $1 &>/dev/null
}

wget:download() {
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		RESUME_CMD="-c"
	fi
	DOWNLOAD_OPTS="--passive-ftp --no-directories --tries=3 --waitretry=3 \
		--directory-prefix=$PKGMK_SOURCE_DIR \
		--output-document=$LOCAL_FILENAME_PARTIAL --no-check-certificate"
	wget $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_WGET_OPTS $1
}

wget:cat() {
	wget -O - $1 2>/dev/null
}

ftp:download() {
	
	check_command "$PKGMK_DOWNLOAD_TOOL"
	
	LOCAL_FILENAME=`get_filename $1`
	LOCAL_FILENAME_PARTIAL="$LOCAL_FILENAME.partial"
	
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		info "Partial download found, trying to resume."
	fi
	
	error=1
	
	BASENAME=`get_basename $1`
	for REPO in ${PKGMK_SOURCE_MIRRORS[@]}; do
		REPO="`echo $REPO | sed 's|/$||'`"
		$PKGMK_DOWNLOAD_TOOL:download $REPO/$BASENAME
		error=$?
		if [[ $error == 0 ]]; then
			break
		fi
	done
	
	if [[ $error != 0 ]]; then
		while true; do
			$PKGMK_DOWNLOAD_TOOL:download $1
			error=$?
			if [[ $error != 0 ]] && [[ "$RESUME_CMD" ]]; then
				info "Partial download failed, restarting."
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
