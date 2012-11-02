recursive() {
	local ARGS FILE DIR
	
	# Hey, we don’t want to make a recursive fail.
	ARGS=("${@//--recursive/}")
	ARGS=("${ARGS[@]//-r/}")
	
	# Now, for each directory that contains a Pkgfile, we move to it and
	# we launch pkg++ again, with it’s args.
	for FILE in `find $PKGMK_ROOT -name $PKGMK_PKGFILE_NAME | sort`; do
		DIR="`dirname $FILE`/"
		if [[ -d $DIR ]]; then
			info "$msg_entering_dir" "${DIR}"
			(cd $DIR && $PKGMK_COMMAND ${ARGS[@]})
			info "$msg_leaving_dir" "${DIR}"
		fi
	done
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
