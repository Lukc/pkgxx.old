
pkgutils:build () {
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

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
