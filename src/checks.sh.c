
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
}

