
PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} nhopkg)
PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} nhopkg)
PKGMK_PM_NEEDS_GROUP=(${PKGMK_PM_NEEDS_GROUP[@]} __PM_NAME)

# nhopkg specific functions.
nhopkg:_nhoid () {
	echo "#%NHO-0.5"
	echo "#%PKG++-${PKGMK_VERSION}"
	echo "# Package Maintainer: `echo "$maintainer" | sed -e "s|(|<|;s|)|>|"`"
	echo "# Name:	$pkgname"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "# Version:	999.${PKGMK_REVISION:-$(date +%Y%m%d)}"
	else
		echo "# Version:	$version"
	fi
	echo "# Release:	$release"
	echo "# License:	$license"
	echo "# Group:	${groups[0]}"
	echo "# Arch:	$PKGMK_ARCH"
	echo "# Dep(post):	${depends[@]}"
	echo "# Installed-Size:	$size"
	# Hum…  we don’t keep that data, for now
	echo "# Build-Duration:	0 min"
	echo "# Build-Date:	$(date -u "+%F %T %z")"
	echo "# Build-Host:	$(hostname)"
	echo "# Url:	$url"
	echo "# Description:	$description"
	echo "# MD5:	$(md5sum data.tar.bz2)"

# I hate that damned noemptyfuncs function… which is in more useless.
# 
# We will need to adapt that later, to really use these post install and
# pre install functions.
	cat << EOT
npostinstall() {
	:
}
EOT
	cat << EOT
npostremove() {
	:
}
EOT
}

nhopkg:target() {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$pkgname-devel-`date +%Y%m%d`-$release.nho"
	else
		echo "$PKGMK_PACKAGE_DIR/$pkgname-$version-$release.nho"
	fi
}

nhopkg:build() {
	cd $PKG
	size="`du -cb . | tail -n 1 | awk '{print $1}'`"
	tar cvvjf data.tar.bz2 *
	nhopkg:_nhoid > nhoid
	info "Building $TARGET."
	tar cf $TARGET nhoid data.tar.bz2
}

nhopkg:footprint() {
	tar xf "${TARGET}" data.tar.bz2
	tar:list "data.tar.bz2" | footprint_sed | sort -k 3
	rm data.tar.bz2
}

nhopkg:install() {
	echo "nhopkg ${PKGMK_INSTALL_ROOT:+--root=$PKGMK_INSTALL_ROOT} -i $TARGET"
}

