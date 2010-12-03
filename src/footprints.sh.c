
#define __FP_SED sed \
-e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
-e "s|	link	to	| -> |" \
-e "s|	->	| -> |"

#define __FP_GTAR(__TARGET) tar tvvf __TARGET | \
sed 's|  *|	|g' | \
cut -d "	" -f 1,2,6,7,8

#define __FP_BSDTAR(__TARGET) bsdtar tvf __TARGET | \
sed 's|  *|	|g' | \
cut -d "	" -f 1,3,4,9,10,11,12

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
				sed '/^@@/d' | \
				sed '/^+++/d' | \
				sed '/^---/d' | \
				sed 's/^+/NEW       /g' | \
				sed 's/^-/MISSING   /g' > $FILE.footprint.diff
			if [[ -s $FILE.footprint.diff ]] && [[ "$version" != "devel" ]]; then
				if [[ "$PKGMK_IGNORE_NEW" = "yes" ]] && \
				   [[ -z "$(egrep -l ^MISSING $FILE.footprint.diff)" ]]; then
					info "New files found:"
				else
					error "Footprint mismatch found:"
					BUILD_SUCCESSFUL="no"
				fi
				cat $FILE.footprint.diff >&2
			fi
		else
			warning "Footprint not found, creating new."
			mv $FILE.footprint $PKGMK_FOOTPRINT
		fi
	else
		error "Package '$TARGET' was not found."
		BUILD_SUCCESSFUL="no"
	fi
}

update_footprint() {
	if [[ ! -f $TARGET ]]; then
		error "Unable to update footprint. File '$TARGET' not found."
		exit E_GENERAL
	fi
	
	check_file "$PKGMK_FOOTPRINT"
	make_footprint > $PKGMK_FOOTPRINT
	touch $TARGET
	
	info "Footprint updated."
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
