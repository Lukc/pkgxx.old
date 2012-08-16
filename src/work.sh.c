
make_work_dir() {
	/*
	 * We export some useful variables, to allow packagers to use them.
	 */
	export PKG="$PKGMK_WORK_DIR/pkg"
	export SRC="$PKGMK_WORK_DIR/src"
	export SPLITS="$PKGMK_WORK_DIR/splits"
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
	
	/* 
	 * Make a repository by split, one after the other.
	 */
	mkdir -p $SRC $PKG
	for SPLIT in ${splits[@]} ${AUTOSPLITS[@]}; do
		mkdir -p "$SPLITS/$SPLIT"
	done
	
	/*
	 * We check md5 and sha256 control sums if possible. If the needed 
	 * tools are not available, it is not possible.
	 */
	if [[ "$PKGMK_IGNORE_CONTROL_SUMS" = "no" ]]; then
		check_control_sums
	fi
}

remove_work_dir() {
	/*
	 * Very easy to understand, I think.
	 */
	rm -rf $PKGMK_WORK_DIR
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
