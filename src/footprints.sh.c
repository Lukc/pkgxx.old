
make_footprint() {
	case $PKGMK_PACKAGE_MANAGER in
		pacman|pacman-g2)
	/*
	 * FIXME: Using three “grep” is not very clean. It should be possible
	 *        to use a pattern instead.
	 */
			#if defined gtar
			tar tvvJf $TARGET | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,2,6,7,8 | \
				grep -v "\.PKGINFO" | \
				grep -v "\.FILELIST" | \
				grep -v "\.CHANGELOG" | \
				sed -e "s|\./\.CHANGELOG||" \
				    -e "s|	->	| -> |" \
				    -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
				sort -k 3
			#elif defined bsdtar
			bsdtar tvJf $TARGET | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,3,4,9,10,11 | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				grep -v "\.PKGINFO" | \
				grep -v "\.FILELIST" | \
				grep -v "\.CHANGELOG" | \
				sed -e "s|\./\.CHANGELOG||" \
				    -e "s|	->	| -> |" \
				    -e "s|	link	to	| -> |" \
				    -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
				sort -k 3
			#else
			#	error No valid tar defined.
			#endif
		;;
		pkgtools)
			#if defined gtar
			tar tvvJf $TARGET | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,2,6,7,8 | \
				grep -v -e "\./$" | \
				grep -v "slack-desc" | \
				grep -v "doinst.sh" | \
				grep -v "drwxr-xr-x	root/root	./install/" | \
				sed -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	->	| -> |" \
				    -e "s|\./||" | \
				sort -k 3
			#elif defined bsdtar
			bsdtar tvJf $TARGET | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,3,4,9,10,11 | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				grep -v -e "\./$" | \
				grep -v "slack-desc" | \
				grep -v "doinst.sh" | \
				grep -v "drwxr-xr-x	root/root	./install/" | \
				sed -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	link	to	| -> |" \
				    -e "s|	->	| -> |" \
				    -e "s|\./||" | \
				sort -k 3
			#else
			#	error No valid tar defined.
			#endif
		;;
		dpkg)
	/*
	 * This is a very dirty method to remove the first line of dpkg’s 
	 * output using “tail”.
	 */
			local footprint=$(
			dpkg -c $TARGET | \
				sed "s|  *|\t|g" | \
				cut -d "	" -f 1,2,6,7,8,9 | \
				sed -e "s|\./||" \
				    -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	link	to	| -> |" | \
				sort -k 3)
			local lines=`echo "$footprint" | wc -l`
			echo "$footprint" | tail -n $(($lines-1))
		;;
		rpm)
			local FILE
			local IFS="
"
			for LINE in $(rpm -qvlp $TARGET | \
				sed "s|  *|\t|g" | \
				cut -d "	" -f 1,3,4,9,10,11 | \
				sed -e "s|/||;s|	|/|;s|	|/|;s|/|	|;s|	->	| -> |" \
				    -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
				sort -k 3)
			do
				FILE="`echo "$LINE" | cut -d '	' -f 3`"
				if [[ -d $PKG/$FILE ]]; then
					echo "$LINE/"
				else
					echo "$LINE"
				fi
			done
		;;
		nhopkg)
			#if defined gtar
			tar xf $TARGET data.tar.bz2
			tar tvvjf data.tar.bz2 | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,2,6,7,8 | \
				sed -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	->	| -> |" \
				    -e "s|\./||" | \
				sort -k 3
			#elif defined bsdtar
			bsdtar xf $TARGET data.tar.bz2
			bsdtar tvjf data.tar.bz2 | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,3,4,9,10,11 | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				sed -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	link	to	| -> |" \
				    -e "s|	->	| -> |" \
				    -e "s|\./||" | \
				sort -k 3
			#else
			#	error No valid tar defined.
			#endif
			rm data.tar.bz2
		;;
		opkg)
			local FOOTPRINT=$(
			#if defined gtar
			tar xf $TARGET data.tar.gz
			tar tvvzf data.tar.gz | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,2,6,7,8 | \
				sed -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	->	| -> |" \
				    -e "s|\./||" | \
				sort -k 3
			#elif defined bsdtar
			bsdtar xf $TARGET data.tar.gz
			bsdtar tvzf data.tar.gz | \
				sed "s|  *|	|g" | \
				cut -d "	" -f 1,3,4,9,10,11 | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				sed -e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
				    -e "s|	link	to	| -> |" \
				    -e "s|	->	| -> |" \
				    -e "s|\./||" | \
				sort -k 3
			#else
			#	error No valid tar defined.
			#endif
			)
			local LINES=$(echo "$FOOTPRINT" | wc -l)
			echo "$FOOTPRINT" | tail -n $(($LINES-1))
			rm data.tar.gz
		;;
		pkgutils)
	/*
	 * This is exactly the same thing, but pkginfo can do the work faster.
	 * However, pkginfo is not available everywhere and for any type of 
	 * package, so pkg++ must be able do to without it on any distribution
	 * that doesn’t use the pkgutils.
	 */
			pkginfo --footprint $TARGET | \
				sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
				sort -k 3
		;;
	esac
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

