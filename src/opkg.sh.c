
opkg:build () {
	/*
	 * We use the opkgmk.sh script, installed by default.
	 */
	cd "$PKG"
	mkdir OPK
	make_debian_control > OPK/control
	_LIBEXECDIR/pkg++/opkgmk.sh -pkg "$TARGET" -root "$PKG" -verbose
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
