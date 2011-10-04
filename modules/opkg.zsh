# FIXME: This module is broken. Any help to make it working would be
# appreciated...

PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} opkg)
PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} opkg)

opkg:check () {
	if ! istrue $FORCE_OPKG; then
		error "The 'opkg' module is broken! The produced packages are considered corrupted."
		error "If you really want to use it, export FORCE_OPKG=true."
		exit 1
	fi
}

opkg:target () {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.opk"
	else
		echo "$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.opk"
	fi
}

opkg:build () {
	# We use the opkgmk.sh script, installed by default.
	cd "$PKG"
	mkdir OPK
	dpkg:_control > OPK/control
	info "Building $TARGET."
	_LIBEXECDIR/pkg++/opkgmk.sh -pkg "$TARGET" -root "$PKG" -verbose
}

opkg:footprint () {
	tar xf "$TARGET" data.tar.gz
	local FOOTPRINT=$(
		#tar:list "${TARGET}" XXX: WTH?
		tar:list data.tar.gz | \
			footprint_sed \
				-e "s|\./||" | \
				sort -k 3
	)
	local LINES=$(echo "$FOOTPRINT" | wc -l)
	echo "$FOOTPRINT" | tail -n $(($LINES-1))
	rm data.tar.gz
}

opkg:install () {
	echo "opkg-cl ${PKGMK_INSTALL_ROOT:+-o $PKGMK_INSTALL_ROOT} install $TARGET"
}

