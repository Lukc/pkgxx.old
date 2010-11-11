
install_package() {
	local COMMAND
	
	#if defined dpkg
	COMMAND="dpkg -i $TARGET"
	#elif defined rpm
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		COMMAND="rpm --nodeps -i $TARGET"
	else
		COMMAND="rpm --nodeps -F $TARGET"
	fi
	#elif defined pacman
		#if defined pacmang2
		if [[ "$PKGMK_INSTALL" = "install" ]]; then
			COMMAND="pacman-g2 -A $TARGET"
		else
			COMMAND="pacman-g2 -U $TARGET"
		fi
		#else
		/*
		 * The correct parameter for pacman is always -U, even if the
		 * package has never been installed.
		 */
		COMMAND="pacman -U $TARGET"
		#endif
	#elif defined pkgtools
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		COMMAND="installpkg $TARGET"
	else
		COMMAND="upgradepkg $TARGET"
	fi
	#elif defined nhopkg
	COMMAND="nhopkg -i $TARGET"
	#else
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		COMMAND="pkgadd $TARGET"
	else
		COMMAND="pkgadd -u $TARGET"
	fi
	#endif
	
	info "Installing '$TARGET'."
	
	cd $PKGMK_ROOT
	echo "$COMMAND"
	$COMMAND
	
	if [[ $? = 0 ]]; then
		info "Installing '$TARGET' succeeded."
	else
		error "Installing '$TARGET' failed."
		exit 1
	fi
}

