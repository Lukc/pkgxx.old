
opkg:build () {
	/*
	 * We use the opkgmk.sh script, installed by default.
	 */
	cd "$PKG"
	mkdir OPK
	make_debian_control > OPK/control
	_LIBEXECDIR/pkg++/opkgmk.sh -pkg "$TARGET" -root "$PKG" -verbose
}

opkg:footprint () {
	local FOOTPRINT=$(
	#if defined gtar
	tar xf $TARGET data.tar.gz
	__FP_GTAR(data.tar.gz) | \
		__FP_SED | \
		    -e "s|\./||" | \
		sort -k 3
	#elif defined bsdtar
	bsdtar xf $TARGET data.tar.gz
	// z
	__FP_BSDTAR(data.tar.gz) | \
		sed "s|	|/|;s|	|/|;s|/|	|" | \
		__FP_SED -e "s|\./||" | \
		sort -k 3
	#else
	#	error No valid tar defined.
	#endif
	)
	local LINES=$(echo "$FOOTPRINT" | wc -l)
	echo "$FOOTPRINT" | tail -n $(($LINES-1))
	rm data.tar.gz
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
