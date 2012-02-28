
#<
# ## FTP — File Transfert Protocol ##
# 
# This module defines how to fetch sources from `HTTP(s)` and 
# `FTP(s)` protocols, using different tools. The tools used
# and how they should be used can be defined in your configuration
# file `pkg++.conf.in`.
# 
# SSL warnings are ignored. If a download fails, downloading only
# the missing parts of the required file will be tried if the 
# download tool used permits it.
# 
# If you have a problem trying to fetch tarballs, look at the
# *Extra configuration* section.
# 
# ### Extra configuration ###
# 
#   * `PKGMK_DOWNLOAD_TOOL="string"` —
#     This variable defines which tool to use to fetch sources. The 
#     supported tools are `curl`, `axel` and `wget`.
#   * `PKGMK_CURL_OPTS="string"` —
#     This variable allows giving additional options to the `curl`
#     executable when downloading a file with curl.
#   * `PKGMK_WGET_OPTS="string"` —
#     This variable allows giving additional options to the `wget`
#     executable when downloading a file with wget.
#   * `PKGMK_AXEL_OPTS="string"` —
#     This variable allows giving additional options to the `axel`
#     executable when downloading a file with axel.
# 
#>

curl:download() {
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		RESUME_CMD="-C $LOCAL_FILENAME_PARTIAL"
	fi
	DOWNLOAD_OPTS=(
		-f
		-L                 # Follow links
		--insecure         # Do not warn about SSL
	)
	curl $RESUME_CMD ${DOWNLOAD_OPTS[@]} $PKGMK_CURL_OPTS "$1" -o "$LOCAL_FILENAME_PARTIAL"
}

curl:cat() {
	curl -L --insecure $1 2>/dev/null
}

axel:download() {
	# axel tries to resume downloads when the output file already 
	# exists… so no need to manage that. ;)
	DOWNLOAD_OPTS=(
			-o "$LOCAL_FILENAME_PARTIAL" # Output
			-a
	)
	axel $RESUME_CMD $DOWNLOAD_OPTS $PKGMK_AXEL_OPTS $1
}

axel:cat() {
	local tmp="`mktemp`"
	axel --output "$tmp" $1 &>/dev/null
	cat "$tmp"
}

wget:download() {
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		RESUME_CMD="-c"
	fi
	# Well, it’s not really a string, and zsh dislikes having strings when it wants arrays of strings.
	DOWNLOAD_OPTS=(
		--passive-ftp          # Some strange things about FTP
		--no-directories
		--tries=3              # Retry if failure
		--waitretry=3          # Time between two tries
		--directory-prefix="$PKGMK_SOURCE_DIR"
		--output-document="$LOCAL_FILENAME_PARTIAL" # Output
		--no-check-certificate # No SSL warn
	)
	wget $RESUME_CMD ${DOWNLOAD_OPTS[@]} $PKGMK_WGET_OPTS $1
}

wget:cat() {
	wget -O- --no-check-certificate - $1 2>/dev/null
}

ftp:download() {
	
	check_command "$PKGMK_DOWNLOAD_TOOL"
	
	LOCAL_FILENAME=`get_filename "$1"`
	LOCAL_FILENAME_PARTIAL="${LOCAL_FILENAME}.partial"
	
	if [[ -f "$LOCAL_FILENAME_PARTIAL" ]]; then
		info "Partial download found, trying to resume."
	fi
	
	error=1
	
	BASENAME=`get_basename $1`
	for REPO in ${PKGMK_SOURCE_MIRRORS[@]}; do
		REPO="`echo $REPO | sed 's|/$||'`"
		${PKGMK_DOWNLOAD_TOOL}:download $REPO/$BASENAME
		error=$?
		if [[ $error == 0 ]]; then
			break
		fi
	done
	
	if [[ $error != 0 ]]; then
		while true; do
			${PKGMK_DOWNLOAD_TOOL}:download "$1"
			error=$?
			if [[ $error != 0 ]] && [[ -n "$RESUME_CMD" ]]; then
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

