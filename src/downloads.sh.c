
download_file() {
	info "Getting '$1'."
	
	PROTOCOL="`get_protocol $1`"
	case $PROTOCOL in
		"http"|"https"|"ftp"|"file")
			if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
				check_command "curl"
			elif [[ "$DOWNLOAD_TOOL" = "wget" ]]; then
				check_command "wget"
			fi
			
			LOCAL_FILENAME=`get_filename $1`
			LOCAL_FILENAME_PARTIAL="$LOCAL_FILENAME.partial"
			
			if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
				DOWNLOAD_OPTS="-f -L --insecure"
			elif [[ "$DOWNLOAD_TOOL" = "wget" ]]; then
				DOWNLOAD_OPTS="--passive-ftp --no-directories --tries=3 --waitretry=3 \
					--directory-prefix=$PKGMK_SOURCE_DIR \
					--output-document=$LOCAL_FILENAME_PARTIAL --no-check-certificate"
			fi
			
			if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
				info "Partial download found, trying to resume"
				if [[ "$DOWNLOAD_TOOL" = "curl" ]]; then
					RESUME_CMD="-C $LOCAL_FILENAME_PARTIAL"
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
		;;
		"svn")
			check_command "svn"
			cd $PKGMK_SOURCE_DIR
			if cd $name 2&> /dev/null; then
				if [[ "`LANG=en_US.utf8 svn up`" =~ 'At revision '*'.' ]]; then
					info "Already up-to-date."
					exit 0
				fi
			else
				svn co `echo $1 | sed -e "s|svn:||"` $name
			fi
		;;
		"git"|"hg")
			cd $PKGMK_SOURCE_DIR
			if cd $name 2&> /dev/null; then
				if [[ "$PROTOCOL" = "git" ]]; then
					check_command "git"
					if [[ "`git pull`" = "Already up-to-date." ]] && [[ "$PKGMK_FORCE" != "yes" ]]; then
						info "Already up-to-date."
						exit 0
					fi
				else
					check_command "hg"
					if [[ "`hg pull | tail -n 1`" = "no changes found" ]] && [[ "$PKGMK_FORCE" != "yes" ]]; then
						info "Already up-to-date."
						exit 0
					else
						hg update > /dev/null
					fi
				fi
			else
				$PROTOCOL clone `echo $1 | sed -e "s|$PROTOCOL:||"` $name
			fi
		;;
		"bzr")
			check_command "bzr"
			cd $PKGMK_SOURCE_DIR
			if cd $name 2&> /dev/null; then
				if [[ "`bzr update 2>&1`" =~ "Tree is up to date at revision ".*" of branch ".* ]] && [[ "$PKGMK_FORCE" != "yes" ]]; then
					info "Already up-to-date."
					exit 0
				fi
			else
				bzr branch `echo $1 | sed -e "s|$PROTOCOL:||"` $name
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

/* vim:setsyntax=sh shiftwidth=4 tabstop=4: */
