
unpack_source() {
	/*
	 * unpack_source() unpacks any file in ${source[ ]}.
	 */
	local FILE LOCAL_FILENAME COMMAND
	
	if [[ -n "${source[@]}" ]]; then
		info "Getting sources of '$name'."
	fi
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME="`get_filename $FILE`"
		/*
		 * $no_extraction can be set to true or yes in Pkgfiles. If so,
		 * pkg++ doesn’t extract any archive nor uncompress any file.
		 */
		if ! istrue "$no_extraction"; then
			case "$LOCAL_FILENAME" in
				*.tar|*.tar.gz|*.tar.Z|*.tgz|*.tar.bz2|*.tbz2|*.tar.xz|*.txz|*.tar.lzma)
					${PKGMK_UNTAR_TOOL}:unpack "$LOCAL_FILENAME" "$SRC"
				;;
				*)
					local LOCAL_EXTENSION="${LOCAL_FILENAME##*\.}"
					if [[ "$(type ${LOCAL_EXTENSION}:unpack)" != "none" ]]; then
						if ! ${LOCAL_EXTENSION}:unpack "${LOCAL_FILENAME}" "${SRC}"; then
							error "Unpacking of '$FILE' failed."
						fi
					else
						subinfo "Copying $LOCAL_FILENAME."
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
