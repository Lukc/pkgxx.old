
unpack_source() {
	local FILE LOCAL_FILENAME COMMAND
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		/*
		 * $keep_archives can be set to true or yes in Pkgfiles. If so,
		 * pkg++ doesn’t extract any archive nor uncompress any file.
		 */
		if ! [[ "$no_extraction" =~ ("true"|"yes") ]]; then
			case $LOCAL_FILENAME in
				#if defined gtar
				*.tar.gz|*.tar.Z|*.tgz)
					COMMAND="tar -C $SRC --use-compress-program=gzip -xf $LOCAL_FILENAME" ;;
				*.tar.bz2|*.tbz2)
					COMMAND="tar -C $SRC --use-compress-program=bzip2 -xf $LOCAL_FILENAME" ;;
				*.tar.xz|*.txz|*.tar.lzma)
					COMMAND="tar -C $SRC --use-compress-program=xz -xf $LOCAL_FILENAME";;
				*.tar)
					COMMAND="tar -C $SRC -xf $LOCAL_FILENAME";;
				*.zip)
					COMMAND="unzip -qq -o -d $SRC $LOCAL_FILENAME" ;;
				#else
				*.tar.gz|*.tar.Z|*.tgz|*.tar.bz2|*.tbz2|*.tar.xz|*.txz|*.tar.lzma|*.zip|*.rpm)
					COMMAND="bsdtar -p -o -C $SRC -xf $LOCAL_FILENAME" ;;
				#endif
				*)
					COMMAND="cp -r $LOCAL_FILENAME $SRC" ;;
			esac
			
			echo "$COMMAND"
			
			$COMMAND
		fi
		
		if [[ $? != 0 ]]; then
			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi
			error "Building '$TARGET' failed."
			exit $E_UNPACK
		fi
	done
}

