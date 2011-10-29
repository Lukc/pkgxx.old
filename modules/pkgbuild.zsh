
# Module to build packages with Arch’s PKGBUILDs. 

PKGMK_RECIPES=(${PKGMK_RECIPES[@]} pkgbuild)

pkgbuild:match () {
	PKGMK_NOFAIL="yes"
	[[ "$PKGMK_PKGFILE" =~ (.*/)*PKGBUILD ]]
}

pkgbuild:parse () {
	eval "$(pkgbuild.bash "$PKGMK_ROOT/$1")"
}

