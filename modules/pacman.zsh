
PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} pacman pacman-g2)
PKGMK_PM_NEEDS_GROUP=(${PKGMK_PM_NEEDS_GROUP[@]} pacman pacman-g2)
# pacman-g2 does not support noarch packages
PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} pacman)

pacman:_pkginfo() {
	# Note: All variables in the .PKGINFO file of the fpm don’t need
	# to be escaped by quotes or double-quotes.
	echo "# Generated by pkg++ $PKGMK_VERSION"
	
	# Name and version of the package.
	# The version is in fact the version and the release of the package, 
	# using the form version-release. It is the same thing on Frugalware’s
	# repositories.
	echo "pkgname = $name"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "pkgver = 999.`date +%Y%m%d`-$release"
	else
		echo "pkgver = $version-$release"
	fi
	
	# Description and URL are not mandatory.
	if [[ -n "$description" ]]; then
		echo "pkgdesc = $description"
	fi
	if [[ -n "$url" ]]; then
		echo "url = $url"
	fi
	
	# The package construction date is mandatory. It is only the result of
	# the “date” command.
	echo -n "builddate = "
	date
	
	# In future, packages will maybe not be built on host but on chroot.
	# The correct value for chrooted builds is “chroot”.
	echo "buildtype = host"
	if [[ -n "$packager" ]]; then
		echo "packager = $packager"
	fi
	if [[ -n "$maintainer" ]]; then
		echo "maintainer = $maintainer"
	fi
	
	# $size is declared in the build_package() function.
	echo "size = $size"
	
	# The architectures on which the package will run. We write here the 
	# architecture of the current host, except if archs[] contains 
	# no-arch, in which case we write here all known architectures. One 
	# line per architecture.
	if [[ "$PKGMK_PACKAGE_MANAGER" = pacman-g2 ]]; then
	# We admit here that $PKGMK_ARCH has been adapted before and
	# is now conform to Frugalware’s standards.
		echo "arch = $PKGMK_ARCH"
	else
	
	# If we use pacman and not pacman-g2, we can create a noarch package.
		if has no-arch ${archs[@]}; then
	# We use “any” to have a noarch package on Arch.
			echo "arch = any"
		else
	# We admit here that $PKGMK_ARCH has been adapted before and 
	# is now conforming to Arch’s standards.
			echo "arch = $PKGMK_ARCH"
		fi
	fi
	# It is possible to keep licence names in the package…
	for LICENSE in ${licenses[@]}; do
		echo "license = $LICENSE"
	done
	
	# We need one line per group, dependency provided package, conflict or
	# backuped file.
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

pacman:_dotinstall() {
	local nonzero=false
	if [[ -n "$PRE_INSTALL" ]]; then
		echo "pre_install() {"
		echo "$PRE_INSTALL" | sed -e "s/^/	/"
		echo "}"
		nonzero=true
	fi
	if [[ -n "$POST_INSTALL" ]]; then
		echo "post_install() {"
		echo "$POST_INSTALL" | sed -e "s/^/	/"
		echo "}"
		nonzero=true
	fi
	if [[ -n "$PRE_REMOVE" ]]; then
		echo "pre_remove() {"
		echo "$PRE_REMOVE" | sed -e "s/^/	/"
		echo "}"
		nonzero=true
	fi
	if [[ -n "$POST_REMOVE" ]]; then
		echo "post_remove() {"
		echo "$POST_REMOVE" | sed -e "s/^/	/"
		echo "}"
		nonzero=true
	fi
	if $nonzero; then
		echo 'op=$1'
		echo 'shift 1'
		echo '$op $*'
		return 0
	else
		return 1
	fi
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
	local dotinstall metafiles
	metafiles=(.FILELIST .PKGINFO)
	cd $PKG
	
	# Frugalware’s packages are very close from Crux’s 
	# ones. The only difference is the presence of some 
	# metadata files at the root of the archive.
	# Three files are needed: .CHANGELOG, .FILELIST and
	# .PKGINFO
	
	# We get the size of the future package’s content.
	size="`du -cb . | tail -n 1 | awk '{print $1}'`"
	
	# We write the files list in the future package.
	find . | sed "s|\./||" | sort > $PKG/.FILELIST
	
	# We write all other informations in the package.
	pacman:_pkginfo > .PKGINFO
	unset size
	
	# Concatened {pre,post}-{install,remove} scripts.
	dotinstall="$(pacman:_dotinstall)"
	if [[ -n "${dotinstall}" ]]; then
		echo -n "${dotinstall}" > $PKG/.INSTALL
		metafiles=(${metafiles[*]} .INSTALL)
	fi
	
	# We install the changelog at the good place, if available.
	if [[ -e "$PKGMK_ROOT/$PKGMK_CHANGELOG" ]]; then
		cp "$PKGMK_ROOT/$PKGMK_CHANGELOG" $PKG/.CHANGELOG
		metafiles=(${metafiles[*]} .CHANGELOG)
	fi
	
	# We choose the good “tarflags” for the compression.
	local TARFLAGS
	case "$PKGMK_COMPRESSION_MODE" in
		gz) TARFLAGS=z ;;
		bz2) TARFLAGS=j ;;
		xz) TARFLAGS=J ;;
		*)
			warning "Unknown compression '$PKGMK_COMPRESSION_MODE'. Using xz."
			TARFLAGS=J
		;;
	esac
	
	# And then we build the package.
	info "Building $TARGET."
	tar:pack "${TARGET}" ${metafiles[*]} * && \
	tar:list "${TARGET}"
}
pacman-g2:build () {
	pacman:build
}

pacman:footprint () {
	tar:list "${TARGET}" | \
		footprint_sed -e '/root\/root	\.CHANGELOG$/d' \
		         -e '/root\/root	\.PKGINFO$/d' \
		         -e '/root\/root	\.INSTALL$/d' \
			 -e '/root\/root	\.FILELIST$/d' | \
		sort -k 3
}
pacman-g2:footprint () {
	pacman:footprint
}

pacman:install() {
	# The correct parameter for pacman is always -U, even if the
	# package has never been installed.
	echo "pacman ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -U $TARGET"
}
pacman-g2:install () {
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		echo "pacman-g2 ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -A $TARGET"
	else
		echo "pacman-g2 ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -U $TARGET"
	fi
}
