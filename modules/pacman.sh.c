
pkg_manager_add(pacman)
pkg_manager_add(pacman-g2)
pkg_manager_noarch(pacman) /* pacman-g2 does not support noarch packages */
pkg_manager_needs_group(pacman)
pkg_manager_needs_group(pacman-g2)

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
	for FILE in ${config[@]}; do
		echo "backup = $FILE"
	done
}

pacman:target() {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		if [[ "$PKGMK_COMPRESSION_MODE" = "none" ]]; then
			echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.pkg.tar"
		else
			echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.pkg.tar.$PKGMK_COMPRESSION_MODE"
		fi
	else
		if [[ "$PKGMK_COMPRESSION_MODE" = "none" ]]; then
			echo "$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.pkg.tar"
		else
			echo "$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.pkg.tar.$PKGMK_COMPRESSION_MODE"
		fi
	fi
}

pacman-g2:target() {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.fpm"
	else
		echo "$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.fpm"
	fi
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
	
	/* 
	 * We install the changelog at the good place, if available.
	 */
	if [[ -e "$PKGMK_ROOT/$PKGMK_CHANGELOG" ]]; then
		cp "$PKGMK_ROOT/$PKGMK_CHANGELOG" $PKG/.CHANGELOG
	fi
	
	/*	
	 * We choose the good “tarflags” for the compression.
	 */
	local TARFLAGS
	case "$PKGMK_COMPRESSION_MODE" in
		gz) TARFLAGS=z ;;
		bz2) TARFLAGS=j ;;
		xz) TARFLAGS=J ;;
		*)
			warning "Unknown compression '$PKGMK_COMPRESSION_MODE'. Using xz."
		;;
	esac
	
	/*
	 * And then we build the package.
	 */
	info "Building $TARGET."
	tar:pack "${TARGET}" .FILELIST .PKGINFO * && \
	tar:list "${TARGET}"
}
pacman-g2:build () {
	pacman:build
}

pacman:footprint () {
	tar:list "${TARGET}" | \
		__FP_SED -e '/^\.\/\.CHANGELOG$/d' \
		         -e '/^\.\/\.PKGINFO$/d' \
			 -e '/^\.\/\.FILELIST$/d' | \
		sort -k 3
}
pacman-g2:footprint () {
	pacman:footprint
}

pacman:install() {
	/*
	 * The correct parameter for pacman is always -U, even if the
	 * package has never been installed.
	 */
	echo "pacman ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -U $TARGET"
}
pacman-g2:install () {
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		echo "pacman-g2 ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -A $TARGET"
	else
		echo "pacman-g2 ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -U $TARGET"
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
