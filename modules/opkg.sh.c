
pkg_manager_add(opkg)

opkg:target () {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.opk"
	else
		echo "$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.opk"
	fi
}

opkg:build () {
	/*
	 * We use the opkgmk.sh script, installed by default.
	 */
	cd "$PKG"
	mkdir OPK
	make_debian_control > OPK/control
	info "Building $TARGET."
	_LIBEXECDIR/pkg++/opkgmk.sh -pkg "$TARGET" -root "$PKG" -verbose
}

opkg:footprint () {
	local FOOTPRINT=$(
	#if defined __GTAR
	tar xf $TARGET data.tar.gz
	__FP_GTAR(data.tar.gz) | \
		__FP_SED | \
		    -e "s|\./||" | \
		sort -k 3
	#elif defined __BSDTAR
	bsdtar xf $TARGET data.tar.gz
	// z
	__FP_BSDTAR(data.tar.gz) | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		__FP_SED -e "s|\./||" | \
		sort -k 3
	#else
	#	error No valid tar defined.
	#endif
	)
	local LINES=$(echo "$FOOTPRINT" | wc -l)
	echo "$FOOTPRINT" | tail -n $(($LINES-1))
	rm data.tar.gz
}

opkg:install () {
	echo "opkg-cl ${PKGMK_INSTALL_ROOT:+-o $PKGMK_INSTALL_ROOT} install $TARGET"
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
