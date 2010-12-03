
make_pacman_pkginfo() {
	/*
	 * The '#' is here to avoid preprocessor and syntaxic coloration
	 * problems.
	 */
	/*
	 * Note: All variables in the .PKGINFO file of the fpm don’t need
	 * to be escaped by quotes or double-quotes.
	 */
	echo '#'" Generated by pkg++ $PKGMK_VERSION"
	/*
	 * Name and version of the package.
	 * The version is in fact the version and the release of the package, 
	 * using the form version-release. It is the same thing on Frugalware’s
	 * repositories.
	 */
	echo "pkgname = $name"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "pkgver = 999.`date +%Y%m%d`-$release"
	else
		echo "pkgver = $version-$release"
	fi
	/*
	 * Description and URL are not mandatory.
	 */
	if [[ -n "$description" ]]; then
		echo "pkgdesc = $description"
	fi
	if [[ -n "$url" ]]; then
		echo "url = $url"
	fi
	/*
	 * The package construction date is mandatory. It is only the result of
	 * the “date” command.
	 */
	echo -n "builddate = "
	date
	/*
	 * In future, packages will maybe not be built on host but on chroot.
	 * The correct value for chrooted builds is “chroot”.
	 */
	echo "buildtype = host"
	if [[ -n "$packager" ]]; then
		echo "packager = $packager"
	fi
	if [[ -n "$maintainer" ]]; then
		echo "maintainer = $maintainer"
	fi
	/*
	 * $size is declared in the build_package() function.
	 */
	echo "size = $size"
	/*
	 * The architectures on which the package will run. We write here the 
	 * architecture of the current host, except if archs[] contains 
	 * no-arch, in which case we write here all known architectures. One 
	 * line per architecture.
	 */
	if [[ "$PKGMK_PACKAGE_MANAGER" = pacman-g2 ]]; then
	/*
	 * We admit here that $PKGMK_ARCH has been adapted before and
	 * is now conform to Frugalware’s standards.
	 */
		echo "arch = $PKGMK_ARCH"
		else
	/*
	 * If we use pacman and not pacman-g2, we can create a noarch package.
	 */
		if has no-arch ${archs[@]}; then
	/*
	 * We use “any” to have a noarch package on Arch.
	 */
			echo "arch = any"
		else
	/*
	 * We admit here that $PKGMK_ARCH has been adapted before and 
	 * is now conform to Arch’s standards.
	 */
			echo "arch = $PKGMK_ARCH"
		fi
	fi
	/*
	 * It is possible to keep licence names in the package…
	 */
	for LICENSE in ${licenses[@]}; do
		echo "license = $LICENSE"
	done
	/*
	 * We need one line per group, dependency provided package, conflict or
	 * backuped file.
	 */
	for GROUP in ${groups[@]}; do
		echo "group = $GROUP"
	done
	for DEP in ${depends[@]}; do
		echo "depend = $DEP"
	done
	for CONFLICT in ${conflicts[@]}; do
		echo "conflict = $CONFLICT"
	done
	for PROVIDE in ${provides[@]}; do
		echo "provides = $PROVIDE"
	done
	for FILE in ${backup[@]}; do
		echo "backup = $FILE"
	done
}

pacman:build() {
	cd $PKG
	/*
	 * Frugalware’s packages are very close from Crux’s 
	 * ones. The only difference is the presence of some 
	 * metadata files at the root of the archive.
	 * Three files are needed: .CHANGELOG, .FILELIST and
	 * .PKGINFO
	 */
	/*
	 * We get the size of the future package’s content.
	 */
	size="`du -cb . | tail -n 1 | awk '{print $1}'`"
	/*
	 * We write the files list in the future package.
	 */
	find . | sed "s|\./||" | sort > .FILELIST
	/*
	 * We write all other informations in the package.
	 */
	make_pacman_pkginfo > .PKGINFO
	unset size
	/* FIXME: What about the Changelog ? :/ */
	/*
	 * And then we build the package.
	 */
	#if defined gtar
		tar cvvf ${TARGET%.$EXT} .FILELIST .PKGINFO *
	#elif defined bsdtar
		bsdtar cf ${TARGET%.$EXT} .FILELIST .PKGINFO *
		bsdtar tvf ${TARGET%.$EXT}
	#else
	#	error No valid tar defined.
	#endif
}
pacman-g2:build () {
	pacman:build
}

pacman:footprint () {
	/*
	 * FIXME: Using three “grep” is not very clean. It should be possible
	 *        to use a pattern instead.
	 */
	#if defined gtar
	__FP_GTAR($TARGET) | \
		grep -v "\.PKGINFO" | \
		grep -v "\.FILELIST" | \
		grep -v "\.CHANGELOG" | \
		__FP_SED -e "s|\./\.CHANGELOG||" \
		sort -k 3
	#elif defined bsdtar
	__FP_BSDTAR($TARGET) | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		grep -v "\.PKGINFO" | \
		grep -v "\.FILELIST" | \
		grep -v "\.CHANGELOG" | \
		__FP_SED -e "s|\./\.CHANGELOG||" \
		sort -k 3
	#else
	#	error No valid tar defined.
	#endif
}
pacman-g2:footprint () {
	pacman:footprint
}

pacman:install() {
	if [[ "$PKGMK_PACKAGE_MANAGER" = pacman-g2 ]]; then
		if [[ "$PKGMK_INSTALL" = "install" ]]; then
			echo "pacman-g2 -A $TARGET"
		else
			echo "pacman-g2 -U $TARGET"
		fi
	else
	/*
	 * The correct parameter for pacman is always -U, even if the
	 * package has never been installed.
	 */
		echo "pacman -U $TARGET"
	fi
}
pacman-g2:install () {
	pacman:install
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
