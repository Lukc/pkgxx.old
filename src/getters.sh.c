
get_protocol() {
	/*
	 * It just returns what is before the first “:”.
	 */
	local PROTOCOL="`echo $1 | cut -d ':' -f 1`"
	echo $PROTOCOL
}
get_filename() {
	/*
	 * Returns the name of the file/directory, even in an URL.
	 */
	if
		[[ $1 =~ ^(http|https|ftp):\/\/.*/(.+) ]] || \
		[[ $1 =~ file:\/\/.* ]] || \
		[[ $1 =~ (svn|git|hg|bzr):.*:\/\/.* ]]
	then
		local NORMAL_RETURN="$PKGMK_SOURCE_DIR/${BASH_REMATCH[2]}"
		local PROTOCOL="`get_protocol $1`"
		case $PROTOCOL in
			svn|git|hg|bzr)
				echo "$PKGMK_SOURCE_DIR/$name"
			;;
			file)
				echo -n "$PKGMK_SOURCE_DIR/"
				echo "$1" | sed -e "s|file://||"
			;;
			*)
				echo "$NORMAL_RETURN"
			;;
		esac
	else
		echo $1
	fi
}

get_basename() {
	/*
	 * Returns something. Probably the complete path of a file on a server.
	 */
	local FILE="`echo $1 | sed 's|^.*://.*/||g'`"
	echo $FILE
}

get_pkgfile() {
	/*
	 * Returns the name of the Pkgfile.
	 */
	local PKGFILE=
	if [[ -n "$PKGMK_PKGFILE" ]]; then
		echo "$PKGMK_PKGFILE"
	elif [[ -n `ls $PKGMK_PKGFILE_NAME-* 2>/dev/null` ]]; then
		for file in $PKGMK_PKGFILE_NAME-*; do
			if [[ "$file" =~ $PKGMK_PKGFILE_NAME-[0-9] ]]; then
				PKGFILE=$file
			fi
		done
		echo "$PKGFILE"
	else
		echo "$PKGMK_PKGFILE_NAME"
	fi
}

get_metafile() {
	/*
	 * Returns the name of any metafile.
	 */
	local VERSION="`basename "$PKGMK_PKGFILE" | sed -e "s|$PKGMK_PKGFILE_NAME-||"`"
	local DIR="`dirname "$PKGMK_PKGFILE"`"
	if [[ "$VERSION" = `basename $PKGMK_PKGFILE` ]]; then
		echo "$DIR/.$1"
	else
		echo "$DIR/.$1-$VERSION"
	fi
}

get_use_desc() {
	local use="$1"
	local FILE
	(
		if [[ -e _SHAREDIR/pkg++/uses ]]; then
			grep "$use" _SHAREDIR/pkg++/uses
		fi
		if [[ -n `ls _SYSCONFDIR/pkg++/uses/` ]]; then
			for FILE in _SYSCONFDIR/pkg++/uses/\*; do
				grep "$use" $FILE
			done
		fi
	) | tail -n 1 \\
	  | cut -d "|" -f 2
}

get_target() {
	case $PKGMK_PACKAGE_MANAGER in
		dpkg)
			if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
				TARGET="$PKGMK_PACKAGE_DIR/${name}_devel-`date +%Y%m%d`-${release}_$PKGMK_ARCH.deb"
			else
				TARGET="$PKGMK_PACKAGE_DIR/${name}_$version-${release}_$PKGMK_ARCH.deb"
			fi
		;;
		rpm)
			if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
				TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.rpm"
			else
				TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.rpm"
			fi
		;;
		opkg)
			if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
				TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.opk"
			else
				TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.opk"
			fi
		;;
		pacman|pacman-g2)
	/*
	 * The extensions change between pacman and pacman-g2.
	 */
			if [[ "PKGMK_PACKAGE_MANAGER" = pacman-g2 ]]; then
				EXT="fpm"
			else
				EXT="pkg.tar.xz"
			fi
			if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
				TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.$EXT"
			else
				TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.$EXT"
			fi
		;;
		pkgtools)
			if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
				TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$PKGMK_ARCH-$release.txz"
			else
				TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$PKGMK_ARCH-$release.txz"
			fi
		;;
		nhopkg)
			if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
				TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release.nho"
			else
				TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release.nho"
			fi
		;;
		pkgutils)
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
				TARGET="$TARGET.$EXT"
			fi
		;;
	esac
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
