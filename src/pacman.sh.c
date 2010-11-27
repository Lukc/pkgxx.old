
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

