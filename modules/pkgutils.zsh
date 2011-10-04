
PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} pkgutils)
PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} pkgutils)

pkgutils:target () {
	local ARCH=""
	
	# Pkgutils users will be able to choose their compression method.
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
	
	if [[ "$distribution" != "Crux" ]]; then
		ARCH=-$PKGMK_ARCH
	fi
	
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name#devel-`date +%Y%m%d`-$release$ARCH.pkg.tar"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name#$version-$release$ARCH.pkg.tar"
	fi
	
	if [[ -n "$EXT" ]]; then
		echo "$TARGET.$EXT"
	else
		echo "$TARGET"
	fi
}

pkgutils:build () {
	info "Building $TARGET."
	cd $PKG
	tar:pack "${TARGET%.$PKGMK_COMPRESSION_MODE}" *
	tar:list "${TARGET%.$PKGMK_COMPRESSION_MODE}"
	
	# pkgutils users have the choice of the compression method.
	# Now this choice will affect their fate.
	case $PKGMK_COMPRESSION_MODE in
		gz)
			gzip -f ${TARGET%.$PKGMK_COMPRESSION_MODE}
		;;
		bz2)
			bzip2 -f ${TARGET%.$PKGMK_COMPRESSION_MODE}
		;;
		xz)
			xz -f ${TARGET%.$PKGMK_COMPRESSION_MODE}
		;;
		lzo)
			lzop -Uf ${TARGET%.$PKGMK_COMPRESSION_MODE}
		;;
	esac
}

pkgutils:footprint () {
	# This is exactly the same thing, but pkginfo can do the work faster.
	# However, pkginfo is not available everywhere and for any type of 
	# package, so pkg++ must be able do to without it on any distribution
	# that doesn’t use the pkgutils.
	pkginfo --footprint $TARGET | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
}

pkgutils:install () {
	if [[ "$PKGMK_INSTALL" = "install" ]]; then
		echo "pkgadd ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} $TARGET"
	else
		echo "pkgadd ${PKGMK_INSTALL_ROOT:+--root $PKGMK_INSTALL_ROOT} -u $TARGET"
	fi
}

