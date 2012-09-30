clean() {
	# clean() removes the sources in $PKGMK_SOURCE_DIR and the package, if
	# it is in $PKGMK_PACKAGE_DIR.
	local FILE LOCAL_FILENAME
	
	for LOCAL_FILENAME in $(list_splits); do
		if [[ -f "$LOCAL_FILENAME" ]]; then
			info "Removing $LOCAL_FILENAME"
			rm -f "$LOCAL_FILENAME"
		fi
	done
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		if [[ -e $LOCAL_FILENAME && "$LOCAL_FILENAME" != "$FILE" ]]; then
			info "Removing $LOCAL_FILENAME"
			if [[ -d $LOCAL_FILENAME ]]; then
				rm -r -f $LOCAL_FILENAME
			else
				rm -f $LOCAL_FILENAME
			fi
		fi
	done
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
