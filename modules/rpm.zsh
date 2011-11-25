
PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} rpm)
PKGMK_PM_NEEDS_GROUP=(${PKGMK_PM_NEEDS_GROUP[@]} rpm)

rpm:_spec() {
	: ${license:="Unknown"}
	: ${buildroot:="$prefix"}
	
	echo "Summary:   $description"
	echo "Name:      $pkgname"
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		echo "Version:   999.${PKGMK_REVISION:-`date +%Y%m%d`}"
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
		sed "s|^\.||"`
	do
		if [[ -d "$PKG/$file" ]]; then
			echo "%dir $file"
		else
			echo "$file"
		fi
	done
	echo
	if [[ -n "$PRE_INSTALL" ]]; then
		echo "%pre"
		echo "$PRE_INSTALL" | sed "/^[ 	]*$/d"
		echo
	fi
	if [[ -n "$POST_INSTALL" ]]; then
		echo "%post"
		echo "$POST_INSTALL" | sed "/^[ 	]*$/d"
		echo
	fi
	if [[ -n "$PRE_REMOVE" ]]; then
		echo "%preun"
		echo "$PRE_REMOVE" | sed "/^[ 	]*$/d"
		echo
	fi
	if [[ -n "$POST_REMOVE" ]]; then
		echo "%postun"
		echo "$POST_REMOVE" | sed "/^[ 	]*$/d"
		echo
	fi
}

rpm:arch() {
	echo "${ARCH}" | sed -e "s|i.86|i386|"
}

rpm:target() {
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		echo "$PKGMK_PACKAGE_DIR/$pkgname-devel-${PKGMK_REVISION:-`date +%Y%m%d`}-$release${rpm_distepoch:+-$rpm_distepoch}-$PKGMK_ARCH.rpm"
	else
		echo "$PKGMK_PACKAGE_DIR/$pkgname-$version-$release${rpm_distepoch:+-$rpm_distepoch}-$PKGMK_ARCH.rpm"
	fi
}

rpm:build() {
	cd $PKG
	rpm:_spec > $PKGMK_WORK_DIR/$pkgname.spec
	# If there is a problem, it’s RPM’s fault.
	rpmbuild --define "_topdir $PKGMK_PACKAGE_DIR/RPM" --quiet --buildroot=$PKG -bb $PKGMK_WORK_DIR/$pkgname.spec
	if [[ "$version" =~ (devel|dev|trunk) ]]; then
		mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$pkgname-999.${PKGMK_REVISION:-`date +%Y%m%d`}-$release${rpm_distepoch:+-$rpm_distepoch}.$PKGMK_ARCH.rpm $TARGET
	else
		mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$pkgname-$version-$release${rpm_distepoch:+-$rpm_distepoch}.$PKGMK_ARCH.rpm $TARGET
	fi
	info "Building $TARGET."
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
		footprint_sed | \
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

rpm:install() {
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		echo "rpm ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} --nodeps -i $TARGET"
	else
		echo "rpm ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} --nodeps -F $TARGET"
	fi
}

rpm:checks() {
	# These vars are vital for RPM to build a package, so we first 
	# check everything in the recipe is ok.
	if [[ "$PKGMK_PACKAGE_MANAGER" = rpm ]]; then
		if [[ -z "$description" ]]; then
			error "Variable 'description' not specified in $PKGMK_PKGFILE."
			exit E_PKGFILE
		elif [[ -z "$packager" ]]; then
			error "Variable 'packager' not specified in $PKGMK_PKGFILE."
			exit E_PKGFILE
		fi
	fi
	
	# And then we check that the particular RPM directories, files, 
	# and so on, are present where they should, plus the recuperation
	# of the distepoch var.
	if [[ $(rpm --version | awk '{print $3}') > 5.1.6 ]] ; then
		export rpm_distepoch="$(rpm --eval '%disttag%distepoch')"
	fi
	check_command rpm
	check_command rpmbuild
	check_directory "$PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH"
}

rpm:unpack() {
	info "Unpacking $1."
	case "$PKGMK_UNRPM_TOOL" in
		bsdtar)
			bsdtar -p -o -C $2 -xf $1
		;;
		rpm)
			error "Hey, we don't already support that!"
			error "If you can help, please, send a patch."
			exit E_GENERAL
		;;
	esac
}

# Note : There is no rpm:pack(), as rpm:build() already exists and should
#        be used.

rpm:list() {
	# FIXME
	:
}

