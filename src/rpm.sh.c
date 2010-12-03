
make_rpm_spec() {
	: ${license:="Unknown"}
	: ${buildroot:="$prefix"}
	
	echo "Summary:   $description"
	echo "Name:      $name"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "Version:   999.`date +%Y%m%d`"
	else
		echo "Version:   $version"
	fi
	echo "Release:   $release"
	echo "License:   $license"
	echo "Packager:  $packager"
	echo "Group:     ${groups[@]}"
	echo "BuildRoot: $buildroot"
	if [[ -n "${depends[@]}" ]]; then
		echo "Requires:  ${depends[@]}"
	fi
	echo 
	echo "%description"
	echo "$description"
	echo 
	echo "%files"
	cd $PKG
	for file in `find . | \
		sed "s|\.||"`
	do
		if [[ -d $file ]]; then
			echo "%dir $file"
		else
			echo "$file"
		fi
	done
}

rpm:build() {
	cd $PKG
	/* 
	 * If there is a problem, it’s RPM’s fault.
	 */
	make_rpm_spec > $PKGMK_WORK_DIR/$name.spec
	rpmbuild --define "_topdir $PKGMK_PACKAGE_DIR/RPM" --quiet --buildroot=$PKG -bb $PKGMK_WORK_DIR/$name.spec
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$name-999.`date +%Y%m%d`-$release.$PKGMK_ARCH.rpm $TARGET
	else
		mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$name-$version-$release.$PKGMK_ARCH.rpm $TARGET
	fi
	rpm -qvlp $TARGET
}

rpm:footprint() {
	local FILE
	local IFS="
"
	for LINE in $(rpm -qvlp $TARGET | \
		sed "s|  *|\t|g" | \
		cut -d "	" -f 1,3,4,9,10,11 | \
		sed -e "s|/||;s|	|/|;s|	|/|;s|/|	|" | \
		__FP_SED | \
		sort -k 3)
	do
		FILE="`echo "$LINE" | cut -d '	' -f 3`"
		if [[ -d $PKG/$FILE ]]; then
			echo "$LINE/"
		else
			echo "$LINE"
		fi
	done
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
