
make_rpm_spec() {
	: ${license:="Unknown"}
	: ${buildroot:="$prefix"}
	/* Using a lot of echos is not adapted, here. */
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

/* vim:setsyntax=sh shiftwidth=4 tabstop=4: */
