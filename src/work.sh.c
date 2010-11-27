
make_work_dir() {
	/*
	 * We export some useful variables, to allow packagers to use them.
	 */
	export PKG="$PKGMK_WORK_DIR/pkg"
	export SRC="$PKGMK_WORK_DIR/src"
	export WORK="$PKGMK_WORK_DIR"
	/*
	 * And we make sure the umask will not cause problem.
	 */
	umask 022
	
	/*
	 * We make the directories.
	 */
	cd $PKGMK_ROOT
	remove_work_dir
	mkdir -p $SRC $PKG
	
	/*
	 * We check md5 and sha256 control sums if possible. If the needed 
	 * tools are not available, it is not possible.
	 */
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
	/*
	 * Very easy to understand, I think.
	 */
	rm -rf $PKGMK_WORK_DIR
}

/* vim:setsyntax=sh shiftwidth=4 tabstop=4: */
