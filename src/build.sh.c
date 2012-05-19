
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

autosplit() {
	for SPLIT in ${AUTOSPLITS[@]}; do
		if [[ "$(type autosplits:${SPLIT}:split)" != "none" ]]; then
			autosplits:${SPLIT}:split
		fi
	done
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
		warning "$msg_should_build_as_root"
	fi
	
	/*
	 * And after though to the poor users, we can extract the sources, if
	 * they need so.
	 */
	unpack_source
	
	/*
	 * Think to the poor user who would not know what is happening whithout
	 * this small line.
	 */
	info "$msg_building_package" "$name" "$(print_useflags)"
	
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
		info "$msg_testing" "$name"
		if check; then
			info "$msg_tests_success" "$name"
		else
			error "$msg_tests_fail" "$name"
			exit E_BUILD
		fi
	fi
	
	/*
	 * If something went wrong
	 */
	if [[ $RETURN != 0 ]]; then
		error "$msg_build_failed" "$name"
		if [[ "$PKGMK_KEEP_ERRLOGS" != "no" && -n "${errlogs[@]}" ]]; then
			for PATTERN in ${errlogs[@]}; do
				for FILE in $(cd $SRC; find . | egrep "${PATTERN}$"); do
					local LOGFILE=$(echo "$FILE" | sed -e "s|/|.|g;s|^\.*||")
					cp $SRC/$FILE $PKGMK_LOGS_DIR/$LOGFILE
					error "$msg_log_avail_at" "$PKGMK_LOGS_DIR/$LOGFILE"
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
		error "$msg_build_failed" "$name"
		exit E_BUILD
	fi
	
	/* 
	 * FIXME: Maybe export of those variables should actually be done sooner,
	 *        in cas the packager wants a for S in ${splits[@]}, or something
	 *        like that. Maybe we should also try to use the distro:class:*
	 *        when available, if nothing else has been defined.
	 */
	for SPLIT in ${AUTOSPLITS[@]}; do
		if [[ "$(type autosplits:${SPLIT}:splitable)" != "none" ]]; then
			if autosplits:${SPLIT}:splitable ]]; then
				splits=(${splits[@]} ${SPLIT})
				
				if [[ "$(type autosplits:${SPLIT}:setsplit)" != "none" ]]; then
					autosplits:${SPLIT}:setsplit
				else
					/* This is kept for easy user-improvised splits */
					eval "${SPLIT}_pkgname=\"${pkgname:-$name}-${SPLIT}\""
				fi
			fi
		else
			warning "$msg_bad_autosplit" "$SPLIT"
		fi
	done
	
	autosplit
	
	/*
	 * We think again to the poor user, as each build will display
	 * a list of files.
	 */
	info "$msg_build_result"
	
	for SPLIT in $name ${splits[@]}; do
		SPLIT="${SPLIT//-/_}"
		
		eval "
			if [[ -z \"\${${SPLIT}_pkgname}\" ]]; then
				if [[ \"${SPLIT}\" == \"${name//-/_}\" ]]; then
					export ${SPLIT}_pkgname=\"${pkgname}\"
				else
					export ${SPLIT}_pkgname=\"${SPLIT}\"
				fi
			fi
		"
		
		for VAR in name version release pkgname description longdesc license url slot class; do
			/* FIXME: Use get_split_data */
			eval "
				export split_${VAR}=\"\$${SPLIT}_${VAR}\"
				if [[ -z \"\$split_${VAR}\" ]]; then
					split_${VAR}=\"\${${VAR}}\"
				fi
			"
		done
		
		for VAR in source depends build_depends archs kernels; do
			eval "
				export split_${VAR}
				split_${VAR}=(\${${SPLIT}_${VAR}[@]})
				if [[ \${#split_${VAR}} = 0 ]]; then
					split_${VAR}=(\${${VAR}[@]})
				fi
			"
		done
		
		/* 
		 * There, we try to use the classes. Classes are meant to make
		 * packages be more compliant with distributions’s standards.
		 */
		if [[ -n "${split_class}" ]]; then
			if [[ "$(type ${distribution}:class:${split_class})" != "none" ]]; then
				${distribution}:class:${split_class} || warning "$msg_func_may_have_failed" "${distribution}:class:${split_class}"
			elif [[ "$(type ${distribution_family}:class:${split_class})" != "none" ]]; then
				${distribution_family}:class:${split_class} || warning "$msg_func_may_have_failed" "${distribution_family}:class:${split_class}"
			fi
		fi
		
		/* 
		 * This is to easy the use of slots on distributions using 
		 * limited package managers and that use hacks to use slots
		 * anyway.
		 */
		if [[ -n "${split_slot}" ]]; then
			if [[ "$(type ${distribution}:slots)" != "none" ]]; then
				${distribution}:slots || warning "$msg_func_may_have_failed" "${distribution}:slots"
			elif [[ "$(type ${distribution_family}:slots)" != "none" ]]; then
				${distribution_family}:slots || warning "$msg_func_may_have_failed" "${distribution_family}:slots"
			fi
		fi
		
		if [[ "${SPLIT}" = "${name//-/_}" ]]; then
			PKG_ROOT="${PKG}"
		else
			PKG_ROOT="${SPLITS}/$SPLIT"
		fi
		
		pkgname="${split_pkgname}"              \
		name="${split_name}"                    \
		version="${split_version}"              \
		release="${split_release}"              \
		description="${split_description}"      \
		longdesc="${split_longdesc}"            \
		license="${split_license}"              \
		url="${split_url}"                      \
		class="${split_class}"                  \
		slot="${split_slot}"                    \
		depends=(${split_depends[@]})           \
		builddepends=(${split_builddepends[@]}) \
		usedepends=(${split_usedepends[@]})     \
		archs=(${split_depends[@]})             \
		kernels=(${split_kernels[@]})           \
		PKG="${PKG_ROOT}"                       \
		ARCH="$(target_arch)"                   \
		KERNEL="$(target_kernel)"               \
		TARGET="$(get_target)"                  \
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
		info "$msg_build_success" "$name"
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
		error "$msg_build_fail" "$name"
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
