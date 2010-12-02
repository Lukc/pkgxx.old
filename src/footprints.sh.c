
#define __FP_SED sed \
-e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
-e "s|	link	to	| -> |" \
-e "s|	->	| -> |"

#define __FP_GTAR tar tvvf $TARGET | \
sed 's|  *|	|g' | \
cut -d "	" -f 1,2,6,7,8

#define __FP_BSDTAR bsdtar tvf $TARGET | \
sed 's|  *|	|g' | \
cut -d "	" -f 1,3,4,9,10,11,12

make_footprint() {
	case $PKGMK_PACKAGE_MANAGER in
		pacman|pacman-g2)
	/*
	 * FIXME: Using three “grep” is not very clean. It should be possible
	 *        to use a pattern instead.
	 */
			#if defined gtar
			__FP_GTAR | \
				grep -v "\.PKGINFO" | \
				grep -v "\.FILELIST" | \
				grep -v "\.CHANGELOG" | \
				__FP_SED -e "s|\./\.CHANGELOG||" \
				sort -k 3
			#elif defined bsdtar
			__FP_BSDTAR | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				grep -v "\.PKGINFO" | \
				grep -v "\.FILELIST" | \
				grep -v "\.CHANGELOG" | \
				__FP_SED -e "s|\./\.CHANGELOG||" \
				sort -k 3
			#else
			#	error No valid tar defined.
			#endif
		;;
		pkgtools)
			#if defined gtar
			// J
			__FP_GTAR | \
				grep -v -e "\./$" | \
				grep -v "slack-desc" | \
				grep -v "doinst.sh" | \
				grep -v "drwxr-xr-x	root/root	./install/" | \
				__FP_SED -e "s|\./||" | \
				sort -k 3
			#elif defined bsdtar
			__FP_BSDTAR | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				grep -v -e "\./$" | \
				grep -v "slack-desc" | \
				grep -v "doinst.sh" | \
				grep -v "drwxr-xr-x	root/root	./install/" | \
				__FP_SED -e "s|\./||" | \
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
				__FP_SED | \
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
				sed -e "s|/||;s|	|/|;s|	|/|;s|/|	|" | \
				__FP_SED | \
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
			__FP_GTAR | \
				__FP_SED -e "s|\./||" | \
				sort -k 3
			#elif defined bsdtar
			bsdtar xf $TARGET data.tar.bz2
			__FP_BSDTAR | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				__FP_SED -e "s|\./||" | \
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
			__FP_GTAR | \
				__FP_SED | \
				    -e "s|\./||" | \
				sort -k 3
			#elif defined bsdtar
			bsdtar xf $TARGET data.tar.gz
			// z
			__FP_BSDTAR | \
				sed "s|	|/|;s|	|/|;s|/|	|" | \
				__FP_SED -e "s|\./||" | \
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
		exit E_GENERAL
	fi
	
	check_file "$PKGMK_FOOTPRINT"
	make_footprint > $PKGMK_FOOTPRINT
	touch $TARGET
	
	info "Footprint updated."
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
