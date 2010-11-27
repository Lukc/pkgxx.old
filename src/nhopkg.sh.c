
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
	echo "# Arch:	$ARCH"
	echo "# Dep(post):	${depends[@]}"
	echo "# Installed-Size:	$size"
	/* Hum…  we don’t keep that data, for now */
	echo "# Build-Duration:	0 min"
	echo "# Build-Date:	$(date -u "+%F %T %z")"
	echo "# Build-Host:	$(hostname -f)"
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


