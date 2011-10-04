
PKGMK_RECIPES=(${PKGMK_RECIPES[@]} pkgfile)

pkgfile:match () {
	# Just a plain, simple regexp.
	[[ "$1" =~ ^(.*/)*Pkgfile(|-.*)$ ]]
}

pkgfile:parse () {
	if [[ "$1" =~ Pkgfile(|-.*)$ ]]; then
		version=${1#*Pkgfile-}
	fi
	source "$1"
}

