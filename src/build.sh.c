
print_useflags() {
	local FLAGS=""
	for use in ${iuse[*]}; do
		if use $use; then
			FLAGS="${FLAGS} ${fg_bold[blue]}${use}${reset_color}"
		else
			case "$?" in
				2)
					FLAGS="${FLAGS} ${fg_bold[yellow]}~${use}${reset_color}"
				;;
				*)
					FLAGS="${FLAGS} ${fg_bold[red]}-${use}${reset_color}"
				;;
			esac
		fi
	done
	if [[ -n "${FLAGS}" ]]; then
		echo -n " [${FLAGS}${fg_bold[white]} ]"
	fi
}

build_package() {
	/*
	 * If the build is not successful, then it is not successful. Logic, 
	 * isn’t it ?
	 */
	local BUILD_SUCCESSFUL="no"
	
	/* 
	 * We will need to give to `set` different parameters, depending on 
	 * the current configuration.
	 */
	local SET_OPTIONS
	
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
	info "Building '$name'$(print_useflags)."
	
	/*
	 * And after though to the poor users, we can extract the sources, if
	 * they need so.
	 */
	unpack_source
	
	/* 
	 * Fail or not in case of bad return, and print or not debug messages.
	 * See bellow for more informations.
	 */
	SET_OPTIONS=
	if [[ "$PKGMK_NOFAIL" != "yes" ]]; then
		SET_OPTIONS="${SET_OPTIONS:--}e"
	fi
	if [[ "$PKGMK_DEBUG" = "yes" ]]; then
		SET_OPTIONS="${SET_OPTIONS:--}x"
	fi
	
	cd $SRC
	/*
	 * The package is built now.
	 * set opts: -e is used to make any error fatal to the subshell.
	 *           -x is used to make the subshell verbose. Any command will
	 *              be displayed with a “+”.
	 */
	if [[ "`type pre_build`" = "function" ]]; then
		([[ -n "$SET_OPTIONS" ]] && set $SET_OPTIONS ; pre_build)
	fi
	local RETURN=$?
	if [[ $RETURN = 0 ]]; then
		([[ -n "$SET_OPTIONS" ]] && set $SET_OPTIONS ; build)
		RETURN=$?
	fi
	if [[ $RETURN = 0 && "`type post_build`" = "function" ]]; then
		([[ -n "$SET_OPTIONS" ]] && set $SET_OPTIONS ; post_build)
		RETURN=$?
	fi
	
	/*
	 * If the user wants to use the check() functions, then we do it for 
	 * them.
	 * Note: check() can make pkg++ consider the package construction as 
	 *       a fail if it fails.
	 */
	if [[ $RETURN != 0 && "$PKGMK_CHECK" = "yes" ]]; then
		info "Testing '$name'."
		if check; then
			info "'$name' successfully tested."
		else
			error "Tests of '$name' failed."
			exit E_BUILD
		fi
	fi
	
	/*
	 * If something went wrong
	 */
	if [[ $RETURN != 0 ]]; then
		error "Building '$name' failed."
		if [[ "$PKGMK_KEEP_ERRLOGS" != "no" && -n "${errlogs[@]}" ]]; then
			for PATTERN in ${errlogs[@]}; do
				for FILE in $(cd $SRC; find . | egrep "${PATTERN}$"); do
					local LOGFILE=$(echo "$FILE" | sed -e "s|/|.|g;s|^\.*||")
					cp $SRC/$FILE $PKGMK_LOGS_DIR/$LOGFILE
					error "Log available at: $PKGMK_LOGS_DIR/$LOGFILE"
				done
			done
		fi
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
		error "Building '$name' failed."
		exit E_BUILD
	fi
	/* 
	 * We export the needed vars to allow splitting.
	 */
	export_splits
	
	/*
	 * We think again to the poor user.
	 */
	info "Build result:"
	for i in {1..${#PKG_NAMES}}; do
		local PKG_ROOT
		if [[ "$i" = 1 ]]; then
			PKG_ROOT="$PKG"
		else
			PKG_ROOT="$SPLITS/${PKG_NAMES[$i]}"
		fi
		split_exec \
			ARCH="$(target_arch)" \
			KERNEL="$(target_kernel)" \
			${PKGMK_PACKAGE_MANAGER}:build
	done
	
	if [[ $? = 0 ]]; then
		BUILD_SUCCESSFUL="yes"
		/*
		 * We check if the package looks like what it should 
		 * be.
		 */
		if [[ ! "$PKGMK_IGNORE_FOOTPRINT" = "yes" ]]; then
			check_footprint
		fi
	fi
	
	/*
	 * If the user doesn’t want to keep explicitly the work dir, we remove 
	 * it.
	 */
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		remove_work_dir
	fi
	
	if [[ "$BUILD_SUCCESSFUL" = "yes" ]]; then
		info "Building '$name' succeeded."
	else
		if [[ -f $TARGET ]]; then
		/*
		 * We touch these files to don’t make pkg++ ignore the package
		 * next time.
		 */
			touch -r $PKGMK_ROOT/$PKGMK_PKGFILE $TARGET &> /dev/null
		fi
		/*
		 * Uh… but if there is no package… uh… pkg++ probably failed.
		 */
		error "Building '$name' failed."
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
