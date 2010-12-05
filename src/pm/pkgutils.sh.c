
pkgutils:target () {
	/*
	 * Pkgutils users will be able to choose their compression method.
	 */
	case $PKGMK_COMPRESSION_MODE in
	gz|bz2|xz|lzo)
		EXT="$PKGMK_COMPRESSION_MODE"
		;;
	none);;
	*)
		error "Compression mode '$PKGMK_COMPRESSION_MODE' not supported"
		exit E_GENERAL
		;;
	esac
	
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name#devel-`date +%Y%m%d`-$release.pkg.tar"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name#$version-$release.pkg.tar"
	fi
	
	if [[ -n "$EXT" ]]; then
		echo "$TARGET.$EXT"
	else
		echo "$TARGET"
	fi
}

pkgutils:build () {
	info "Building $TARGET."
	#if defined gtar
	tar cvvf ${TARGET%.$EXT} *
	#elif defined bsdtar
	bsdtar cf ${TARGET%.$EXT} *
	/*
	 * bsdtar cvvf doesn’t give enough informations about the 
	 * saved files. So we create the archive and then we give to 
	 * the user a files list.
	 * Note: this list is different from the one given by GNU tar
	 *       with cvv.
	 */
	bsdtar tvf ${TARGET%.$EXT}
	#else
	#	error No valid tar defined.
	#endif
	/*
	 * pkgutils users have the choice of the compression method.
	 * Now this choice will affect their fate.
	 */
	case $PKGMK_COMPRESSION_MODE in
		gz)
			gzip -f ${TARGET%.$EXT}
		;;
		bz2)
			bzip2 -f ${TARGET%.$EXT}
		;;
		xz)
			xz -f ${TARGET%.$EXT}
		;;
		lzo)
			lzop -Uf ${TARGET%.$EXT}
		;;
	esac
}

pkgutils:footprint () {
	/*
	 * This is exactly the same thing, but pkginfo can do the work faster.
	 * However, pkginfo is not available everywhere and for any type of 
	 * package, so pkg++ must be able do to without it on any distribution
	 * that doesn’t use the pkgutils.
	 */
	pkginfo --footprint $TARGET | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
}

pkgutils:install () {
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		echo "pkgadd $TARGET"
	else
		echo "pkgadd -u $TARGET"
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
