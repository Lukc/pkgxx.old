
PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} dpkg)
PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} dpkg)

dpkg:_list() {
	while [[ -n "$1" ]]; do
		if [[ -n "${2}" ]]; then
			echo -n "${1},"
		else
			echo -n "${1}"
		fi
		shift 1
	done
	echo
}

dpkg:_control() {
	echo "Package: ${name//_/-}"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "Version: 999.${PKGMK_REVISION:-`date +%Y%m%d`-$release}"
	else
		echo "Version: $version"
	fi
	
	echo "Maintainer: $maintainer"
	
	if [[ -n "${groups[@]}" ]]; then
		echo "Section: ${groups[0]}"
	fi

	echo "Description: $description"
	if [[ -n "$longdesc" ]]; then
		# For dpkg, a blank line is equal to a new field, so we
		# just write a dot. We also insert a space at the begining
		# of each line, else it is also a new field.
		echo "$longdesc" | sed -e "s|^$|.|;s|^| |"
	fi
	echo "Architecture: $(dpkg:arch)"
	echo -n "Depends: "
	if ((${#depends} >= 1)); then
		dpkg:_list ${depends[@]}
	fi
	if ((${#conflicts} >= 1)); then
		echo -n "Conflicts: "
		dpkg:_list ${conflicts[@]}
	fi
	echo "Homepage: $url"
	echo
}

dpkg:checks() {
	check_command dpkg
}

dpkg:arch() {
	case ${KERNEL} in
		freebsd*) ARCH=kfreebsd-$ARCH ;;
		hurd*) ARCH=hurd-$ARCH ;;
		netbsd*) ARCH=netbsd-$ARCH ;;
		# Not sure for the others…
	esac
	#local ARCH=${PKGMK_ARCH:-$(uname -m)}
	if has no-arch ${archs[@]}; then
		# “all” is the keyword for architecture-independent packages, 
		# not “any”.
		echo "all"
		return
	fi
	echo "${ARCH}" | sed -e "s|i.86|i386|;s|x86_64|amd64|" \
	                     -e "s|arm|armel|" \
	                     -e "s|ppc|powerpc|;s|powerpc64|ppc64|"
}

dpkg:target() {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/${name//_/-}_devel-${PKGMK_REVISION:-`date +%Y%m%d`}-${release}_$PKGMK_ARCH.deb"
	else
		echo "$PKGMK_PACKAGE_DIR/${name//_/-}_$version-${release}_$PKGMK_ARCH.deb"
	fi
}

dpkg:build() {
	cd $PKG
	mkdir DEBIAN
	dpkg:_control > DEBIAN/control
	if [[ -n "$PRE_INSTALL" ]]; then
		echo "$PRE_INSTALL" > DEBIAN/preinst
	fi
	if [[ -n "$POST_INSTALL" ]]; then
		echo "$POST_INSTALL" > DEBIAN/postinst
	fi
	if [[ -n "$PRE_REMOVE" ]]; then
		echo "$PRE_REMOVE" > DEBIAN/prerm
	fi
	if [[ -n "$POST_REMOVE" ]]; then
		echo "$POST_REMOVE" > DEBIAN/postrm
	fi
	if [[ -e "$PKGMK_ROOT/$PKGMK_CHANGELOG" ]]; then
		cp "$PKGMK_ROOT/$PKGMK_CHANGELOG" DEBIAN/changelog
	fi
	for FILE in ${config[@]}; do
		[[ -e "$PKG/$FILE" ]] && echo "$FILE" >> DEBIAN/conffiles
	done
	cd ..
	info "Building '$TARGET'."
	dpkg-deb --build "$PKG"
	mv $(basename "$PKG").deb $TARGET
	dpkg -c $TARGET
}

dpkg:footprint() {
	# This is a very dirty method to remove the first line of dpkg’s 
	# output using “tail”.
	local footprint; footprint=$( \
		dpkg -c "$TARGET" | \
		sed "s|  *|	|g" | \
		cut -d "	" -f 1,2,6,7,8,9 | \
		sed -e "s|\./||" | \
		footprint_sed | \
		sort -k 3)
	local lines=`echo "$footprint" | wc -l`
	echo "$footprint" | tail -n $(($lines-1))
}

dpkg:install() {
	echo "dpkg ${PKGMK_INSTALL_ROOT:+--root=$PKGMK_INSTALL_ROOT} -i $TARGET"
}
