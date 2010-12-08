
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

dpkg:arch() {
	case ${ARCH} in
		i?86) ARCH=i386 ;;
		x86_64) ARCH=amd64 ;;
		arm) ARCH=armel ;;
		ppc) ARCH=powerpc ;;
		powerpc64) ARCH=ppc64 ;;
	esac
	case ${KERNEL} in
		freebsd*) ARCH=kfreebsd-$ARCH ;;
		/* Not sure for the others… */
	esac
}

dpkg:target() {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/${name}_devel-`date +%Y%m%d`-${release}_$PKGMK_ARCH.deb"
	else
		echo "$PKGMK_PACKAGE_DIR/${name}_$version-${release}_$PKGMK_ARCH.deb"
	fi
}

dpkg:build() {
	cd $PKG
	mkdir DEBIAN
	make_debian_control > DEBIAN/control
	cd ..
	info "Building $TARGET."
	dpkg-deb --build $PKG
	mv $(basename $PKG).deb $TARGET
	dpkg -c $TARGET
}

dpkg:footprint() {
	/*
	 * This is a very dirty method to remove the first line of dpkg’s 
	 * output using “tail”.
	 */
	local footprint=$(
	dpkg -c $TARGET | \
		sed "s|  *|\t|g" | \
		cut -d "	" -f 1,2,6,7,8,9 | \
		sed -e "s|\./||" | \
		__FP_SED | \
		sort -k 3)
	local lines=`echo "$footprint" | wc -l`
	echo "$footprint" | tail -n $(($lines-1))
}

dpkg:install() {
	echo "dpkg ${PKGMK_INSTALL_ROOT:+--root=$PKGMK_INSTALL_ROOT} -i $TARGET"
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
