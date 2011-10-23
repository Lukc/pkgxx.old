
# Module to build packages with Arch’s PKGBUILDs. 

pkgbuild: () {
	PKGMK_NOFAIL="yes"
	[[ "$PKGMK_PKGFILE" =~ (.*/)*PKGBUILD ]]
}

pkgbuild:parse () {
eval "$(cat $PKGMK_PKGFILE)
export name=$pkgname
export version=$pkgver
export release=$pkgrel
export description=$pkgdesc

export startdir="$PKGMK_WORK_DIR" # May cause problems in future.
export pkgdir="$PKG"
export srcdir="$SRC"

if [[ \"$\(type package\)\" = function ]]; then
	post_build () {
		package
	}
fi
"
}

