
build_package() {
	/*
	 * If the build is not successful, then it is not successful. Logic, 
	 * isn’t it ?
	 */
	local BUILD_SUCCESSFUL="no"
	
	/*
	 * We check if we can create $TARGET, or recreate it if it already 
	 * exists. Then we make the work dir.
	 */
	check_file "$TARGET"
	make_work_dir
	
	/*
	 * Non-root fakeroot builds sometimes make footprints and permissions 
	 * errors.
	 */
	if [[ "$UID" != "0" ]]; then
		warning "Packages should be built as root."
	fi
	
	/*
	 * Think to the poor user who would not know what is happening whithout
	 * this small line.
	 */
	info "Building '$TARGET'."
	
	/*
	 * And after though to the poor users, we can extract the sources, if
	 * they need so.
	 */
	unpack_source
	
	cd $SRC
	/*
	 * The package is built now.
	 * set opts: -e is used to make any error fatal to the subshell.
	 *           -x is used to make the subshell verbose. Any command will
	 *              be displayed with a “+”.
	 */
	if [[ "`type -t pre_build`" = "function" ]]; then
		(set -e `[[ "$PKGMK_DEBUG" = yes ]] && echo "-x"` ; pre_build)
	fi
	local RETURN=$?
	if [[ $RETURN = 0 ]]; then
		(set -e `[[ "$PKGMK_DEBUG" = yes ]] && echo "-x"` ; build)
		RETURN=$?
	fi
	if [[ $RETURN = 0 && "`type -t post_build`" = "function" ]]; then
		(set -e `[[ "$PKGMK_DEBUG" = yes ]] && echo "-x"` ; post_build)
		RETURN=$?
	fi
	
	/*
	 * If the user wants to use the check() functions, then we do it for 
	 * them.
	 * Note: check() can make pkg++ consider the package construction as 
	 *       a fail if it fails.
	 */
	if [[ $RETURN != 0 && "$PKGMK_CHECK" = "yes" ]]; then
		info "Testing $TARGET."
		if check; then
			info "$TARGET successfully tested."
		else
			error "Tests of $TARGET failed."
			exit E_BUILD
		fi
	fi
	
	/*
	 * If something went wrong
	 */
	if [[ $RETURN != 0 ]]; then
		error "build_package() returned $RETURN."
		return $RETURN
	fi
	/*
	 * We strip the binaries/libraries if asked for.
	 */
	if [[ "$PKGMK_NO_STRIP" = "no" ]]; then
		strip_files
	fi
	
	/*
	 * Manual pages compression.
	 */
	compress_manpages
	
	/*
	 * We check here that the package is not empty.
	 */
	cd $PKG
	if [[ "`find . | wc -l`" = 1 ]]; then
		error "Building '$TARGET' failed."
		exit E_BUILD
	fi
	/* 
	 * We export the needed vars to allow splitting.
	 */
	PKG_NAMES=("$name" "${splits[@]}")
	PKG_VERSIONS=("$version" "${splits_version[@]}")
	PKG_LICENSES=("$license" "${splits_licenses[@]}")
	PKG_DESC=("$description" "${splits_descriptions[@]}")
	PKG_ARCHS=("${archs[0]}" "${subarchs[@]}")
	PKG_DEPENDS=(${depends[@]} "${subdepends[@]}")
	/*
	 * We think again to the poor user.
	 */
	info "Build result:"
	for i in ${!PKG_NAMES[*]}; do
		local PKG_ROOT
		if [[ "$i" = 0 ]]; then
			PKG_ROOT="$PKG"
		else
			PKG_ROOT="$SPLITS/${PKG_NAMES[$i]}"
		fi
		name="${PKG_NAMES[$i]}" version="${PKG_VERSIONS[$i]:-$version}" \
		license="${PKG_LICENSES[$i]:-$license}" \
		description="${PKG_DESC[$i]}" archs=(${PKG_ARCHS[$i]}) \
		depends="${PKG_DEPENDS[$i]}" PKG="$PKG_ROOT" \
		TARGET="$(get_target)" \
			$PKGMK_PACKAGE_MANAGER:build
	done
	if [[ "$PKGMK_PACKAGE_MANAGER" =~ pacman|pacman-g2 ]]; then
	/* 
	 * I don’t remember why, but there was a problem with pacmen.
	 * FIXME: This will be a real problem when splits will be implemented…
	 */
	mv ${TARGET%.$EXT}.$PKGMK_COMPRESSION_MODE ${TARGET}
	fi
	
	if [[ $? = 0 ]]; then
		BUILD_SUCCESSFUL="yes"
		/*
		 * We check if the package looks like what it should 
		 * be.
		 */
		if [[ "$PKGMK_IGNORE_FOOTPRINT" = "yes" ]]; then
			warning "Footprint ignored."
		else
			check_footprint
		fi
	fi
	
	/*
	 * If the user don’t want to keep explicitly the work dir, we remove 
	 * it.
	 */
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		remove_work_dir
	fi
	
	if [[ "$BUILD_SUCCESSFUL" = "yes" ]]; then
		info "Building '$TARGET' succeeded."
	else
		if [[ -f $TARGET ]]; then
		/*
		 * We touch these files to avoid rebuilding an up to date 
		 * package.
		 */
			touch -r $PKGMK_ROOT/$PKGMK_PKGFILE $TARGET &> /dev/null
		fi
		/*
		 * Uh… but if there is no package… uh… pkg++ probably failed.
		 */
		error "Building '$TARGET' failed."
		exit E_BUILD
	fi
}

build_needed() {
	/*
	 * build_needed() just checks if $TARGET exists and if it is up to 
	 * date.
	 */
	local FILE RESULT
	
	RESULT="yes"
	if [[ -f $TARGET ]]; then
		RESULT="no"
		for FILE in $PKGMK_PKGFILE ${source[@]}; do
			FILE=`get_filename $FILE`
			if [[ ! -e $FILE ]] || [[ ! $TARGET -nt $FILE ]]; then
				RESULT="yes"
				break
			fi
		done
	fi
	
	echo $RESULT
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
