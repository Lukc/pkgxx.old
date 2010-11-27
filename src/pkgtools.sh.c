
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

