
check_arch(){
	/*
	 * Returns true if the package can be built on the actual architecture.
	 * It uses $PKGMK_ARCH and ${archs[ ]}.
	 */
	if [[ -n "${archs[@]}" ]]; then
		for ARCH in ${archs[@]}; do
			case $ARCH in
			no-arch)
	/* For packages which will run on any arch once built. */
				return 0;;
			all)
	/* For packages which will build on ary arch. */
				return 0;;
			$PKGMK_ARCH)
				return 0;;
			esac
		done
		return 1
	else
		return 0
	fi
}

check_kernel(){
	/*
	 * Exactly the same thing than check_arch(), but for kernels.
	 */
	if [[ -n "${kernels[@]}" ]]; then
		for KERNEL in ${kernels[@]}; do
			case $KERNEL in
			no-kernel)
				return 0;;
			all)
				return 0;;
			$PKGMK_KERNEL)
				return 0;;
			esac
		done
		return 1
	else
		return 0
	fi
}

check_portability() {
	/* 
	 * TODO: Checking for the libc could also be usefull.
	 */
	if ! check_arch; then
		error "This package is not made to work on your hardware."
		exit E_BAD_ARCH
	elif ! check_kernel; then
		error "This package is not made to work on your kernel."
		exit E_BAD_KERNEL
	fi
}

check_pkgfile_core() {
	/*
	 * We check if the recipe is valid and usable.
	 */
	local RETURN=0
	
	check_portability
	
	for variable in name version release; do
		eval "
			if [[ -z \"\${$variable}\" ]]; then
				error \"Variable \\\`${variable}' not specified in \\\`$PKGMK_PKGFILE'.\"
				RETURN=1
			fi
		"
	done
	
	if [[ "`type build`" != "function" ]]; then
		error "Function \`build' not specified in \`$PKGMK_PKGFILE'."
		RETURN=1
	fi
	
	if [[ "$PKGMK_CHECK" = "yes" ]]; then
		if [[ "`type check`" != "function" ]]; then
			warning "Function \`check' not specified in \`$PKGMK_PKGFILE'."
			PKGMK_CHECK="no"
		fi
	fi
	
	return $RETURN
}

/*
 * check_pkgfile_only() checks the Pkgfile and the presence of new variables 
 * that were not included in Crux’s pkgmk.
 * It gives many informations about the recipe itself, and so is very usefull
 * for packagers.
 */
check_pkgfile_full() {
	/*
	 * $RETURN will store the error code returned by pkg++ at the end of 
	 * the check.
	 */
	local RETURN
	
	check_pkgfile_core
	RETURN=$?
	
	/* 
	 * Those variables are not present in Crux’ Pkgfiles, but are used on
	 * many package managers, and their presence is also sometimes mandatory.
	 * Because of that, if one is not declared, a warning or an error is
	 * displayed.
	 */
	for variable in description url packager maintainer license; do
			/* FIXME: Maybe this should be errors? */
		eval "
			if [[ -z \"\${$variable}\" ]]; then
				warning \"Variable \\\`${variable}' not specified in \\\`$PKGMK_PKGFILE'.\"
				RETURN=1
			fi
		"
	done
	
	/*
	 * pre_build() and post_build() are not mandatory. They are 
	 * only used to help packagers who want to use include files
	 * but for who they are uncomplete.
	 * build() however is mandatory.
	 */
	if [[ "`type build`" != "function" ]]; then
		error "Function 'build' not specified in '$PKGMK_PKGFILE'."
		RETURN=1
	fi
	
	if istrue "$PKGMK_CHECK" && [[ "`type check`" != "function" ]]; then
		/*
		 * check() should be given in recipes, to allow a user
		 * to know if everything will work, but this is not mandatory
		 * and sometimes difficult to do.
		 */
		warning "Function 'check' not specified in '$PKGMK_PKGFILE'."
		RETURN=1
	fi
	
	if [[ -n "$lastver" ]]; then
		local last_version="$(eval "$lastver" | sed -e 's/^[	 ]*//;s/[ 	]*$//')"
		if [[ -n "$last_version" ]]; then
			if @{ "$last_version" ">" "$version" }@ ; then
				warning "Last version, '$last_version', is greater than the current version '$version'."
				RETURN=2
			fi
		else
			error "\$last_version did not return anything."
			RETURN=3
		fi
	else
		warning "Variable 'lastver' not specified in '$PKGMK_PKGFILE'."
		RETURN=1
	fi
	
	if [[ $RETURN = 0 ]]; then
		info "Everything is in perfect order."
	fi
	
	exit $RETURN
}

check_directory() {
	/*
	 * Checks if the directory exists, is readable and writable.
	 */
	if [[ ! -d "$1" ]]; then
		error "Directory $1 does not exist."
		exit E_DIR_PERM
	elif [[ ! -w "$1" ]]; then
		error "Directory $1 not writable."
		exit E_DIR_PERM
	elif [[ ! -x "$1" ]] || [[ ! -r "$1" ]]; then
		error "Directory $1 not readable."
		exit E_DIR_PERM
	fi
}

check_file() {
	/*
	 * Checks if a file exists, and if it doesn’t exist, if it is possible
	 * to create it.
	 */
	if [[ -e "$1" ]] && [[ ! -w "$1" ]]; then
		error "File $1 is not writable."
		exit E_DIR_PERM
	fi
}

check_command() {
	/*
	 * Checks if a command is available.
	 */
	if [[ "`type "$1"`" = "none" ]]; then
		error "Command $1 not found."
		exit E_GENERAL
	fi
}

check_config() {
	/*
	 * Checks the usability of the configuration.
	 */
	if ! has $PKGMK_DOWNLOAD_TOOL ${PKGMK_DOWNLOAD_TOOLS[@]}; then
		error "$PKGMK_DOWNLOAD_TOOL is not a valid download tool. Please, edit your config file."
		echo "Valid values are: ${PKGMK_DOWNLOAD_TOOLS[@]}."
		exit E_INVALID_DOWNLOAD_TOOL
	fi
	if ! has "$PKGMK_MAN_COMPRESSION" "gz" "bz2" "xz"; then
		error "$PKGMK_MAN_COMPRESSION is not a valid man pages compression. Please, edit your config file."
		echo "Valid values are: [gz] bz2 xz."
		exit E_INVALID_MAN_COMPRESSION
	fi
	if ! has $PKGMK_UNTAR_TOOL ${PKGMK_UNTAR_TOOLS[@]}; then
		error "The untar tool '$PKGMK_UNTAR_TOOL' is not provided by any module. Please, edit your config file."
		echo "Valid values are: ${PKGMK_UNTAR_TOOLS[@]}"
		exit E_MODULE_NOT_PROVIDED
	fi
	if ! has $PKGMK_PACKAGE_MANAGER ${PKGMK_PACKAGE_MANAGERS[@]}; then
		error "$PKGMK_PACKAGE_MANAGER is not a supported package manager. Please, edit your config file."
		echo "Valid values are: ${PKGMK_PACKAGE_MANAGERS[@]}."
		exit E_INVALID_PM
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
