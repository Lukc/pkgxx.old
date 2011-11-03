
PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} spack)
PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} spack)

spack:checks () {
	check_command spackpkg
	if ! has pkgtools ${PKGMK_PACKAGE_MANAGERS[@]}; then
		error
		error "The pkgtools module is required to write slack-desc."
		error
		exit 1
	fi
}

spack:target () {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$name-devel-${PKGMK_REVISION:=`date +%Y%m%d`}-$PKGMK_ARCH-$release.spack"
	else
		echo "$PKGMK_PACKAGE_DIR/$name-$version-$PKGMK_ARCH-$release.spack"
	fi
}

spack:build () {
	cd $PKG
	mkdir $PKG/install
	pkgtools:_slackspec > $PKG/install/slack-desc
	info "Building $TARGET."
	spackpkg -q "$PKG"
	mv $PKG/../pkg.spack "$TARGET"
}

spack:footprint () {
	warning "Spack footprints are not supported (for now)!"
}

spack:install () {
	echo "spackadd ${PKGMK_INSTALL_ROOT:+-root $PKGMK_INSTALL_ROOT} $TARGET"
}

