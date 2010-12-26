
pkg_manager_add(dpkg)

make_debian_control() {
	echo "Package: $name"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "Version: 999.`date +%Y%m%d`-$release"
	else
		echo "Version: $version"
	fi
	
	echo "Maintainer: $maintainer"
	
	if [[ -n "${groups[@]}" ]]; then
		echo "Section: ${groups[0]}"
	fi

	echo "Description: $description"
	if [[ -n "$long_desc" ]]; then
		/* 
		 * For dpkg, a blank line is equal to a new field, so we
		 * just write a dot. We also insert a space at the begining
		 * of each line, else it is also a new field.
		 */
		echo "$long_desc" | sed -e "s|^$|.|;s|^| |"
	fi
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
	case ${KERNEL} in
		freebsd*) ARCH=kfreebsd-$ARCH ;;
		hurd*) ARCH=hurd-$ARCH ;;
		netbsd*) ARCH=netbsd-$ARCH ;;
		/* Not sure for the others… */
	esac
	if [[ "${ARCH}" = noarch ]]; then
		/* 
		 * “all” is the keyword for architecture-independent packages, 
		 * not “any”.
		 */
		echo "all"
	fi
	echo "${ARCH}" | sed -e "s|i.86|i386|;s|x86_64|amd64|" \
	                     -e "s|arm|armel|" \
	                     -e "s|ppc|powerpc|;s|powerpc64|ppc64|"
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
	if [[ "$PKG_NAME" == "$name" ]]; then
		if [[ -e "$PKGMK_ROOT/$PKGMK_PRE_INSTALL" ]]; then
			cp "$PKGMK_ROOT/$PKGMK_PRE_INSTALL" DEBIAN/preinst
		fi
		if [[ -e "$PKGMK_ROOT/$PKGMK_POST_INSTALL" ]]; then
			cp "$PKGMK_ROOT/$PKGMK_POST_INSTALL" DEBIAN/postinst
		fi
		if [[ -e "$PKGMK_ROOT/$PKGMK_PRE_REMOVE" ]]; then
			cp "$PKGMK_ROOT/$PKGMK_PRE_REMOVE" DEBIAN/prerm
		fi
		if [[ -e "$PKGMK_ROOT/$PKGMK_POST_REMOVE" ]]; then
			cp "$PKGMK_ROOT/$PKGMK_POST_REMOVE" DEBIAN/postrm
		fi
	else
		if [[ -e "$PKGMK_ROOT/$name.$PKGMK_PRE_INSTALL" ]]; then
			cp "$PKGMK_ROOT/$name.$PKGMK_PRE_INSTALL" DEBIAN/preinst
		fi
		if [[ -e "$PKGMK_ROOT/$name.$PKGMK_POST_INSTALL" ]]; then
			cp "$PKGMK_ROOT/$name.$PKGMK_POST_INSTALL" DEBIAN/postinst
		fi
		if [[ -e "$PKGMK_ROOT/$name.$PKGMK_PRE_REMOVE" ]]; then
			cp "$PKGMK_ROOT/$name.$PKGMK_PRE_REMOVE" DEBIAN/prerm
		fi
		if [[ -e "$PKGMK_ROOT/$name.$PKGMK_POST_REMOVE" ]]; then
			cp "$PKGMK_ROOT/$name.$PKGMK_POST_REMOVE" DEBIAN/postrm
		fi
	fi
	if [[ -e "$PKGMK_ROOT/$PKGMK_CHANGELOG" ]]; then
		cp "$PKGMK_ROOT/$PKGMK_CHANGELOG" DEBIAN/changelog
	fi
	for FILE in ${config[@]}; do
		[[ -e "$PKG/$FILE" ]] && echo "$FILE" >> DEBIAN/conffiles
	done
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
