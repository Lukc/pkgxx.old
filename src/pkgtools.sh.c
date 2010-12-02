
make_slackspec () {
	/*
	 * This function creates the “spec” file of the txz.
	 * Known problems : description of more than one line will cause a 
	 * failure somewhere in the pkgtools. (installpkg or upgradepkg)
	 */
	echo "|-----handy-ruler------------------------------------------------------|"
	echo "$name: $description"
	for i in {1..10}; do
		echo "$name:"
	done
}

pkgtools:build () {
	mkdir $PKG/install
	make_slackspec > $PKG/install/slack-desc
	(
		cd $PKG
	/*
	 * We create the package using makepkg. Doing this way
	 * avoid some warnings. We redirect makepkg’s output to
	 * /dev/null to skip it’s verbosity.
	 */
		makepkg -l y -c n $TARGET &> /dev/null
	/*
	 * As makepkg is redirected to /dev/null, we print the 
	 * content of the package with tar.
	 */
		tar tvJf $TARGET
		)
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
