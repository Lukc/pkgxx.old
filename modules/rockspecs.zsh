
PKGMK_RECIPES=(${PKGMK_RECIPES[@]} rockspecs)

rockspecs:match () {
	# If nothing before ".rockspec", then it’s a faaake.
	[[ "$1" =~ .+\.rockspec$ ]]
}

rockspecs:parse () {
	eval "$(xxrocks.lua "$1")"
}

