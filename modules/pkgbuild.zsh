
# Module to build packages with Arch’s PKGBUILDs. See frugalbuild.sh.c
# for the first try of this sort of strange mix. It is basically the 
# same thing, except helpers functions are not declared.
if [[ "$PKGMK_PKGFILE" =~  (.*/)*PKGBUILD ]]; then
	includes=(pkgbuild)
	
	PKGMK_NOFAIL="yes"
fi

