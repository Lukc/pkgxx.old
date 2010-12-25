
pkg_manager_add(nhopkg)

/*
 * nhopkg specific functions.
 */
make_nhoid () {
	echo "#%NHO-0.5"
	echo "#%PKG++-${PKGMK_VERSION}"
	echo "# Package Maintainer: `echo "$maintainer" | sed -e "s|(|<|;s|)|>|"`"
	echo "# Name:	$name"
	echo "# Version:	$version"
	echo "# Release:	$release"
	echo "# License:	$license"
	echo "# Group:	${groups[0]}"
	echo "# Arch:	$PKGMK_ARCH"
	echo "# Dep(post):	${depends[@]}"
	echo "# Installed-Size:	$size"
	/* Hum…  we don’t keep that data, for now */
	echo "# Build-Duration:	0 min"
	echo "# Build-Date:	$(date -u "+%F %T %z")"
	echo "# Build-Host:	$(hostname)"
	echo "# Url:	$url"
	echo "# Description:	$description"
	echo "# MD5:	$(md5sum data.tar.bz2)"

/* 
 * I hate that damned noemptyfuncs function… which is in more useless.
 * 
 * We will need to adapt that later, to really use these post install and
 * pre install functions.
 */
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
		echo "$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release.nho"
	else
		echo "$PKGMK_PACKAGE_DIR/$name-$version-$release.nho"
	fi
}

nhopkg:build() {
	cd $PKG
	size="`du -cb . | tail -n 1 | awk '{print $1}'`"
	tar cvvjf data.tar.bz2 *
	make_nhoid > nhoid
	info "Building $TARGET."
	tar cf $TARGET nhoid data.tar.bz2
}

nhopkg:footprint() {
	#if defined __GTAR
	tar xf $TARGET data.tar.bz2
	__FP_GTAR(data.tar.bz2) | \
		__FP_SED -e "s|\./||" | \
		sort -k 3
	#elif defined __BSDTAR
	bsdtar xf $TARGET data.tar.bz2
	__FP_BSDTAR(data.tar.bz2) | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		__FP_SED -e "s|\./||" | \
		sort -k 3
	#else
	#	error No valid tar defined.
	#endif
	rm data.tar.bz2
}

nhopkg:install() {
	echo "nhopkg ${PKGMK_INSTALL_ROOT:+--root=$PKGMK_INSTALL_ROOT} -i $TARGET"
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
