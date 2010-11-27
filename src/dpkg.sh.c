
make_debian_control() {
	echo "Package: $name"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "Version: 999.`date +%Y%m%d`-$release"
	else
		echo "Version: $version"
	fi
	if has no-arch ${archs[@]}; then
		echo "Architecture: all"
	else
		echo "Architecture: $PKGMK_ARCH"
	fi
	echo "Maintainer: $maintainer"
	echo "Description: $description"
	echo -n "Depends: "
	for n in ${!depends[*]}; do
		if [[ -n "${depends[$(($n+1))]}" ]]; then
			echo -n "${depends[$n]},"
		else
			echo -n "${depends[$n]}"
		fi
	done
	echo
	echo "Homepage: $url"
	echo
}
