
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
					tar:unpack "$LOCAL_FILENAME" "$SRC"
				;;
				*.zip)
					zip:unpack "$LOCAL_FILENAME" "$SRC"
				;;
				#if defined __BSDTAR
				*.rpm)
					COMMAND="bsdtar -p -o -C $SRC -xf $LOCAL_FILENAME"
					echo "$COMMAND"
					$COMMAND
				;;
				#endif
				*)
					info "Copying $LOCAL_FILENAME"
					cp -r "$LOCAL_FILENAME" "$SRC"
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
