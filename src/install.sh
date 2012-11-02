install_package() {
	local COMMAND
	
	COMMAND=($($PKGMK_PACKAGE_MANAGER:install))
	
	info "$msg_installing_info" "${TARGET}"
	
	cd $PKGMK_ROOT
	echo "${COMMAND[*]}"
	${COMMAND[*]}
	
	if [[ $? = 0 ]]; then
		info "$msg_success_install" "${TARGET}"
	else
		error "$msg_failure_install" "${TARGET}"
		exit $E_INSTALL
	fi
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
