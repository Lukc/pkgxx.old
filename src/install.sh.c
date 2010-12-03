
install_package() {
	local COMMAND
	
	COMMAND="$($PKGMK_PACKAGE_MANAGER:install)"
	
	info "Installing '$TARGET'."
	
	cd $PKGMK_ROOT
	echo "$COMMAND"
	$COMMAND
	
	if [[ $? = 0 ]]; then
		info "Installing '$TARGET' succeeded."
	else
		error "Installing '$TARGET' failed."
		exit E_INSTALL
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
