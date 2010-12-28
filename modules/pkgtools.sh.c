
pkg_manager_add(pkgtools)
pkg_manager_noarch(pkgtools)

make_slackspec () {
	/*
	 * This function creates the “spec” file of the txz.
	 */
	local lenght i=0
	echo "|-----handy-ruler------------------------------------------------------|"
	if [[ -z "$long_desc" ]]; then
		echo "$name: $description" | head -n 1
		for i in {1..10}; do
			echo "$name: "
		done
	else
		lenght=$(echo "$long_desc" | wc -l)
		echo "$long_desc" | sed -e "s/^/$name: /" | head -n 11
		while (((11-$lenght) > $i)); do
			echo "$name: "
			((i++))
		done
	fi
}

pkgtools:checks () {
	check_command makepkg
}

pkgtools:target () {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$PKGMK_ARCH-$release.txz"
	else
		echo "$PKGMK_PACKAGE_DIR/$name-$version-$PKGMK_ARCH-$release.txz"
	fi
}

pkgtools:build () {
	mkdir $PKG/install
	make_slackspec > $PKG/install/slack-desc
	info "Building $TARGET."
	(
		cd $PKG
	/*
	 * We create the package using makepkg. Doing this way
	 * avoid some warnings. We redirect makepkg’s output to
	 * /dev/null to skip it’s verbosity.
	 */
		makepkg -l y -c n $TARGET &> /dev/null
	/*
	 * As makepkg is redirected to /dev/null, we print the 
	 * content of the package with tar.
	 */
		tar tvJf $TARGET
		)
}

pkgtools:footprint () {
	#if defined __GTAR
	// J
	__FP_GTAR($TARGET) | \
		grep -v -e "\./$" | \
		grep -v "slack-desc" | \
		grep -v "doinst.sh" | \
		grep -v "drwxr-xr-x	root/root	./install/" | \
		__FP_SED -e "s|\./||" | \
		sort -k 3
	#elif defined __BSDTAR
	__FP_BSDTAR($TARGET) | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		grep -v -e "\./$" | \
		grep -v "slack-desc" | \
		grep -v "doinst.sh" | \
		grep -v "drwxr-xr-x	root/root	./install/" | \
		__FP_SED -e "s|\./||" | \
		sort -k 3
	#else
	#	error No valid tar defined.
	#endif
}

pkgtools:install () {
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		echo "installpkg ${PKGMK_INSTALL_ROOT:+-root $PKGMK_INSTALL_ROOT} $TARGET"
	else
		echo "upgradepkg ${PKGMK_INSTALL_ROOT:+-root $PKGMK_INSTALL_ROOT} $TARGET"
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
