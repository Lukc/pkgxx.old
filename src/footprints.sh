make_footprint() {
	$PKGMK_PACKAGE_MANAGER:footprint
}

check_footprint() {
	local FILE="$PKGMK_WORK_DIR/.tmp"
	
	cd $PKGMK_ROOT
	
	if [[ -f $TARGET ]]; then
		make_footprint > $FILE.footprint
		if [[ -f $PKGMK_FOOTPRINT ]]; then
			sort -k 3 $PKGMK_FOOTPRINT > $FILE.footprint.orig
			diff -w -t -U 0 $FILE.footprint.orig $FILE.footprint | \
				sed '
					/^@@/d
					/^+++/d
					/^---/d
					s/^+/NEW       /g
					s/^-/MISSING   /g
				' > $FILE.footprint.diff
			if [[ -s $FILE.footprint.diff ]] && [[ "$version" != "devel" ]]; then
				if [[ "$PKGMK_IGNORE_NEW" = "yes" ]] && \
				   [[ -z "$(egrep -l ^MISSING $FILE.footprint.diff)" ]]; then
					info "$smsg_new_files_found"
				else
					error "$msg_footprint_mismatch"
					BUILD_SUCCESSFUL="no"
				fi
				cat $FILE.footprint.diff >&2
			fi
		else
			warning "$msg_create_footprint"
			mv $FILE.footprint $PKGMK_FOOTPRINT
		fi
	else
		error "$msg_package_not_found" "${TARGET}"
		BUILD_SUCCESSFUL="no"
	fi
}

update_footprint() {
	if [[ ! -f $TARGET ]]; then
		error "$msg_unable_update_footprint" "${TARGET}"
		exit $E_GENERAL
	fi
	
	check_file "$PKGMK_FOOTPRINT"
	make_footprint > $PKGMK_FOOTPRINT
	touch $TARGET
	
	info "$msg_footprint_updated"
}

footprint_sed() {
	sed "
		s|	lib/modules/`uname -r`/|	lib/modules/<kernel-version>/|g
		s|	link	to	| -> |
		s|	->	| -> |
	" $@
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
