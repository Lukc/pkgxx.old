
make_work_dir() {
	export PKG="$PKGMK_WORK_DIR/pkg"
	export SRC="$PKGMK_WORK_DIR/src"
	umask 022
	
	cd $PKGMK_ROOT
	remove_work_dir
	mkdir -p $SRC $PKG
	
	if [[ "$PKGMK_IGNORE_MD5SUM" = "no" ]]; then
		if [[ -n "`type -p md5sum`" ]]; then
			check_md5sum
		else
			warning "'md5sum' command not found. md5sum verification ignored."
		fi
	fi
	
	if [[ "$PKGMK_IGNORE_SHA256SUM" = "no" ]]; then
		if [[ -n "`type -p sha256sum`" ]]; then
			check_sha256sum
		else
			warning "'sha256sum' command not found. sha256sum verification ignored."
		fi
	fi
}

remove_work_dir() {
	rm -rf $PKGMK_WORK_DIR
}

