
check_arch(){
	/*
	 * Returns true if the package can be built on the actual architecture.
	 * It uses $PKGMK_ARCH and ${archs[ ]}.
	 */
	if [[ "${archs[@]}" ]]; then
		for ARCH in ${archs[@]}; do
			case $ARCH in
			no-arch)
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
	if [[ "${kernels[@]}" ]]; then
		for KERNEL in ${kernels[@]}; do
			case $KERNEL in
			no-kernel)
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

check_pkgfile() {
	/*
	 * We check if the recipe is valid and usable.
	 */
	if [[ ! "$name" ]]; then
		error "Variable 'name' not specified in $PKGMK_PKGFILE."
		exit $E_PKGFILE
	elif [[ ! "$version" ]]; then
		error "Variable 'version' not specified in $PKGMK_PKGFILE."
		exit $E_PKGFILE
	elif [[ ! "$release" ]]; then
		error "Variable 'release' not specified in $PKGMK_PKGFILE."
		exit $E_PKGFILE
	#if defined rpm
	elif [[ ! "$description" ]]; then
		error "Variable 'description' not specified in $PKGMK_PKGFILE."
		exit $E_PKGFILE
	elif [[ ! "$packager" ]]; then
		error "Variable 'packager' not specified in $PKGMK_PKGFILE."
		exit $E_PKGFILE
	#endif
	elif [[ "`type -t build`" != "function" ]]; then
		error "Function 'build' not specified in $PKGMK_PKGFILE."
		exit $E_PKGFILE
	elif ! check_arch; then
		error "This package is not made to work on your harware."
		exit 1
	elif ! check_kernel; then
		error "This package is not made to work on your kernel."
		exit 1
	fi
	if [[ "$PKGMK_CHECK" = "yes" ]]; then
		if [[ ! "`type -t check`" = "function" ]]; then
			warning "Function 'check' not specified in $PKGMK_PKGFILE."
			PKGMK_CHECK="no"
		fi
	fi
}

check_directory() {
	/*
	 * Checks if the directory exists, is readable and writable.
	 */
	if [[ ! -d $1 ]]; then
		error "Directory $1 does not exist."
		exit $E_DIR_PERM
	elif [[ ! -w $1 ]]; then
		error "Directory $1 not writable."
		exit $E_DIR_PERM
	elif [[ ! -x $1 ]] || [[ ! -r $1 ]]; then
		error "Directory $1 not readable."
		exit $E_DIR_PERM
	fi
}

check_file() {
	/*
	 * Checks if a file exists, and if it doesn’t exist, if it is possible
	 * to create it.
	 */
	if [[ -e $1 ]] && [[ ! -w $1 ]]; then
		error "File $1 is not writable."
		exit 1
	fi
}

check_command() {
	/*
	 * Checks if a command is available.
	 */
	if [[ -z "`type -p $1`" ]]; then
		error "Command $1 not found."
		exit $E_GENERAL
	fi
}

check_config() {
	/*
	 * Checks the usability of the configuration.
	 */
	if ! has $DOWNLOAD_TOOL ${PKGMK_DOWNLOAD_TOOLS[@]}; then
		error "$DOWNLOAD_TOOL is not a valid download tool. Please, edit your config file."
		echo "Valid values are: ${PKGMK_DOWNLOAD_TOOLS[@]}."
		exit $E_INVALID_DOWNLOAD_TOOL
	fi
	if ! [[ "$PKGMK_MAN_COMPRESSION" =~ ("gz"|"bz2") ]]; then
		error "$PKGMK_MAN_COMPRESSION is not a valid man pages compression. Please, edit your config file."
		echo "Valid values are: gz bz2."
		exit $E_INVALID_MAN_COMPRESSION
	fi
}

/*
 * check_new_pkgfile() checks the Pkgfile and the presence of new variables 
 * that were not included in Crux’s pkgmk.
 * It gives many informations about the recipe itself, and so is very usefull
 * for packagers.
 * TODO: (0.9/0.10) Check for long descriptions.
 */
check_new_pkgfile () {
	/*
	 * $RETURN will store the error code returned by pkg++ at the end of 
	 * the check.
	 */
	RETURN=0
	/*
	 * $description, $url, $packager, $maintainer and $depends are not 
	 * really Crux-compliant™. However, it is very useful and important
	 * for some package managers, such as dpkg, rpm, pacman or other. So we
	 * print an error here, but pkg++ will run even if these variables are 
	 * not declared in the Pkgfile.
	 */
	if [[ ! "$description" ]]; then
		error "Variable 'description' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$url" ]]; then
		error "Variable 'url' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$packager" ]]; then
		error "Variable 'packager' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$maintainer" ]]; then
		error "Variable 'maintainer' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$depends" ]]; then
		error "Variable 'depends' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$name" ]]; then
		error "Variable 'name' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$version" ]]; then
		error "Variable 'version' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$release" ]]; then
		error "Variable 'release' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ ! "$license" ]]; then
		/*
		 * License can be considered as only useful with unfree 
		 * softwares or particulary restrictive licenses. As it is not
		 * used by many package managers, we just print a warning and
		 * don’t return any error code.
		 */
		warning "Variable 'license' not specified in $PKGMK_PKGFILE."
	fi
	/*
	 * pre_build() and post_build() are not mandatory. They are 
	 * only used to help packagers that want to use include files
	 * but for who they are uncomplete.
	 * build() however is mandatory.
	 */
	if [[ "`type -t pre_build`" != "function" ]]; then
		info "Function 'pre_build' not specified in $PKGMK_PKGFILE."
	fi
	if [[ "`type -t build`" != "function" ]]; then
		error "Function 'build' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	if [[ "`type -t post_build`" != "function" ]]; then
		info "Function 'post_build' not specified in $PKGMK_PKGFILE."
	fi
	if [[ "`type -t check`" != "function" ]]; then
		/*
		 * check() should be given in recipes, to allow a user
		 * to know if everything will work, but this is not mandatory
		 * and sometimes difficult to do.
		 */
		warning "Function 'check' not specified in $PKGMK_PKGFILE."
		RETURN=1
	fi
	exit $RETURN
}

