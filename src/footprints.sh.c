
make_footprint() {
	#if defined fpm
	/*
	 * FIXME: Using three “grep” is not very clean. It should be possible
	 *        to use a pattern instead.
	 */
	#if defined gtar
	tar tvvJf $TARGET | \
		sed "s|  *|	|g" | \
		cut -d "	" -f 1,2,6 | \
		grep -v "\.PKGINFO" | \
		grep -v "\.FILELIST" | \
		grep -v "\.CHANGELOG" | \
		sed "s|\./\.CHANGELOG||" | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
	#else
	bsdtar tvJf $TARGET | \
		sed "s|  *|	|g" | \
		cut -d "	" -f 1,3,4,9,10,11 | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		grep -v "\.PKGINFO" | \
		grep -v "\.FILELIST" | \
		grep -v "\.CHANGELOG" | \
		sed "s|\./\.CHANGELOG||" | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
	#endif
	#elif defined pkgtools
	#if defined gtar
	tar tvvJf $TARGET | \
		sed "s|  *|	|g" | \
		cut -d "	" -f 1,2,6 | \
		grep -v "slack-desc" | \
		grep -v "doinst.sh" | \
		grep -v "drwxr-xr-x	root/root	install/" | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
	#else
	bsdtar tvJf $TARGET | \
		sed "s|  *|	|g" | \
		cut -d "	" -f 1,3,4,9,10,11 | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		grep -v "slack-desc" | \
		grep -v "doinst.sh" | \
		grep -v "drwxr-xr-x	root/root	install/" | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
	#endif
	#elif defined dpkg
	/*
	 * This is a very dirty method to remove the first line of dpkg’s 
	 * output using “tail”.
	 */
	local footprint=`
	dpkg -c $TARGET | \
		sed "s|  *|\t|g" | \
		cut -d "	" -f 1,2,6,7,8 | \
		sed "s|\./||" | \
		sort -k 3`
	local lines=`echo "$footprint" | wc -l`
	echo "$footprint" | tail -n $(($lines-1))
	#elif defined rpm
	rpm -qvlp $TARGET | \
		sed "s|  *|\t|g" | \
		cut -d "	" -f 1,3,4,9,10,11 | \
		sed "s|/||;s|	|/|;s|	|/|;s|/|	|;s|	->	| -> |" | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
	#else
	/*
	 * This is exactly the same thing, but pkginfo can do the work faster.
	 * However, pkginfo is not available everywhere and for any type of 
	 * package, so pkg++ must be able do to without it on any distribution
	 * that doesn’t use the pkgutils.
	 */
	pkginfo --footprint $TARGET | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
	#endif
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
		exit 1
	fi
	
	check_file "$PKGMK_FOOTPRINT"
	make_footprint > $PKGMK_FOOTPRINT
	touch $TARGET
	
	info "Footprint updated."
}

