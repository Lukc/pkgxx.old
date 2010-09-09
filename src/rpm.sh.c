
make_rpm_spec() {
	: ${license:="Unknown"}
	: ${buildroot:="$prefix"}
	/* Using a lot of echos is not adapted, here. */
	cat << EOF
Summary:   $description
Name:      $name
Version:   $version
Release:   $release
License:   $license
Packager:  $packager
Group:     ${groups[@]}
BuildRoot: $buildroot
EOF
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

