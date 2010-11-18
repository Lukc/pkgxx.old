
install_package() {
	local COMMAND
	
	case $PKGMK_PACKAGE_MANAGER in
		dpkg)
	COMMAND="dpkg -i $TARGET"
		;;
		rpm)
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		COMMAND="rpm --nodeps -i $TARGET"
	else
		COMMAND="rpm --nodeps -F $TARGET"
	fi
		;;
		pacman|pacman-g2)
			if [[ "$PKGMK_PACKAGE_MANAGER" = pacman-g2 ]]; then
				if [[ "$PKGMK_INSTALL" = "install" ]]; then
					COMMAND="pacman-g2 -A $TARGET"
				else
					COMMAND="pacman-g2 -U $TARGET"
				fi
			else
		/*
		 * The correct parameter for pacman is always -U, even if the
		 * package has never been installed.
		 */
				COMMAND="pacman -U $TARGET"
			fi
		;;
		pkgtools)
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		COMMAND="installpkg $TARGET"
	else
		COMMAND="upgradepkg $TARGET"
	fi
		;;
		nhopkg)
	COMMAND="nhopkg -i $TARGET"
		;;
		pkgutils)
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		COMMAND="pkgadd $TARGET"
	else
		COMMAND="pkgadd -u $TARGET"
	fi
		;;
	esac
	
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

