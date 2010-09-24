
build_package() {
	/*
	 * If the build is not successful, then it is not successful. Logic, 
	 * isn’t it ?
	 */
	local BUILD_SUCCESSFUL="no"
	
	/*
	 * We check if we can create $TARGET, or recreate it if it already 
	 * exists. Then we make the work dir.
	 */
	check_file "$TARGET"
	make_work_dir
	
	/*
	 * Non-root fakeroot builds sometimes make footprints and permissions 
	 * errors.
	 */
	if [[ "$UID" != "0" ]]; then
		warning "Packages should be built as root."
	fi
	
	/*
	 * Think to the poor user who would not know what is happening whithout
	 * this small line.
	 */
	info "Building '$TARGET'."
	
	/*
	 * And after though to the poor users, we can extract the sources, if
	 * they need so.
	 */
	unpack_source
	
	cd $SRC
	/*
	 * The package is built now.
	 * set opts: -e is used to make any error fatal to the subshell.
	 *           -x is used to make the subshell verbose. Any command will
	 *              be displayed with a “+”.
	 */
	if [[ "`type -t pre_build`" = "function" ]]; then
		(set -e -x ; pre_build)
	fi
	(set -e -x ; build)
	if [[ "`type -t post_build`" = "function" ]]; then
		(set -e -x ; post_build)
	fi
	
	/*
	 * If the user wants to use the check() functions, then we do it for 
	 * them.
	 * Note: check() can make pkg++ consider the package construction as 
	 *       a fail if it fails.
	 */
	if [[ "$PKGMK_CHECK" = "yes" ]]; then
		info "Testing $TARGET."
		if check; then
			info "$TARGET successfully tested."
		else
			error "Tests of $TARGET failed."
			exit 1
		fi
	fi
	
	/*
	 * If everything went well.
	 */
	if [[ $? = 0 ]]; then
		/*
		 * We strip the binaries/libraries if asked for.
		 */
		if [[ "$PKGMK_NO_STRIP" = "no" ]]; then
			strip_files
		fi
		
		/*
		 * Manual pages compression.
		 */
		compress_manpages
		
		/*
		 * We check here that the package is not empty.
		 */
		cd $PKG
		if [[ "`find . | wc -l`" = 1 ]]; then
			error "Building '$TARGET' failed."
			exit 1
		fi
		/*
		 * We think again to the poor user.
		 */
		info "Build result:"
		#if defined dpkg
		/*
		 * We don’t want to know how deb are done, they are, that’s 
		 * enough.
		 */
		mkdir DEBIAN
		[[ -e DEBIAN/control ]] && rm DEBIAN/control
		echo "Package: $name" >> DEBIAN/control
		if [[ "$version" =~ (devel|dev|trunk) ]]; then
			echo "Version: 999.`date +%Y%m%d`" >> DEBIAN/control
		else
			echo "Version: $version" >> DEBIAN/control
		fi
		if has no-arch ${archs[@]}; then
			echo "Architecture: all" >> DEBIAN/control
		else
			echo "Architecture: $PKGMK_ARCH" >> DEBIAN/control
		fi
		echo "Maintainer: $maintainer" >> DEBIAN/control
		echo "Description: $description" >> DEBIAN/control
		echo -n "Depends: " >> DEBIAN/control
		for n in ${!depends[*]}; do
			if [[ -n "${depends[$(($n+1))]}" ]]; then
				echo -n "${depends[$n]},">> DEBIAN/control
			else
				echo -n "${depends[$n]}">> DEBIAN/control
			fi
		done
		echo >> DEBIAN/control
		echo "Homepage: $url" >> DEBIAN/control
		echo >> DEBIAN/control
		cd ..
		dpkg-deb --build $PKG
		mv pkg.deb $TARGET
		dpkg -c $TARGET
		#elif defined rpm
		/*
		 * If there is a problem, it’s RPM’s fault.
		 */
		make_rpm_spec > $PKGMK_WORK_DIR/$name.spec
		rpmbuild --define "_topdir $PKGMK_PACKAGE_DIR/RPM" --quiet --buildroot=$PKG -bb $PKGMK_WORK_DIR/$name.spec
		if [[ "$version" =~ (devel|dev|trunk) ]]; then
			mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$name-999.`date +%Y%m%d`-$release.$PKGMK_ARCH.rpm $TARGET
		else
			mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$name-$version-$release.$PKGMK_ARCH.rpm $TARGET
		fi
		rpm -qvlp $TARGET
		#elif defined pkgtools
		mkdir $PKG/install
		make_slackspec > $PKG/install/slack-desc
		(
			cd $PKG
			/*
			 * We create the package using makepkg. Doing this way
			 * avoid some warnings. We redirect makepkg’s output to
			 * /dev/null to skip it’s verbosity.
			 */
			makepkg -l y -c n $TARGET &> /dev/null
			/*
			 * As makepkg is redirected to /dev/null, we print the 
			 * content of the package with tar.
			 */
			tar tvJf $TARGET
		)
		#else
			#if defined pacman
			/*
			 * Frugalware’s packages are very close from Crux’s 
			 * ones. The only difference is the presence of some 
			 * metadata files at the root of the archive.
			 * Three files are needed: .CHANGELOG, .FILELIST and
			 * .PKGINFO
			 */
			/*
			 * We get the size of the future package’s content.
			 */
			size="`du -cb . | tail -n 1 | awk '{print $1}'`"
			/*
			 * We write the files list in the future package.
			 */
			find . | sed "s|\./||" | sort > .FILELIST
			/*
			 * We write all other informations in the package.
			 */
			make_pacman_pkginfo > .PKGINFO
			unset size
			// FIXME: What about the Changelog ? :/
			/*
			 * And then we build the package.
			 */
			#if defined gtar
				tar cvvf ${TARGET%.$EXT} .FILELIST .PKGINFO *
			#else
				bsdtar cf ${TARGET%.$EXT} .FILELIST .PKGINFO *
				bsdtar tvf ${TARGET%.$EXT}
			#endif
		#else
			#if defined gtar
				tar cvvf ${TARGET%.$EXT} *
			#else
				bsdtar cf ${TARGET%.$EXT} *
		/*
		 * bsdtar cvvf doesn’t give enough informations about the 
		 * saved files. So we create the archive and then we give to 
		 * the user a files list.
		 * Note: this list is different from the one given by GNU tar
		 *       with cvv.
		 */
				bsdtar tvf ${TARGET%.$EXT}
			#endif
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
		#if defined pacman
		/*
		 * I don’t remember why, but there was a problem with pacmen.
		 */
		mv ${TARGET%.$EXT}.$PKGMK_COMPRESSION_MODE ${TARGET}
		#endif
		#endif
		
		if [[ $? = 0 ]]; then
			BUILD_SUCCESSFUL="yes"
			/*
			 * We check if the package looks like what it should 
			 * be.
			 */
			if [[ "$PKGMK_IGNORE_FOOTPRINT" = "yes" ]]; then
				warning "Footprint ignored."
			else
				check_footprint
			fi
		fi
	fi
	
	/*
	 * If the user don’t want to keep explicitly the work dir, we remove 
	 * it.
	 */
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		remove_work_dir
	fi
	
	if [[ "$BUILD_SUCCESSFUL" = "yes" ]]; then
		info "Building '$TARGET' succeeded."
	else
		if [[ -f $TARGET ]]; then
		/*
		 * We touch these files to avoid rebuilding an up to date 
		 * package.
		 */
			touch -r $PKGMK_ROOT/$PKGMK_PKGFILE $TARGET &> /dev/null
		fi
		/*
		 * Uh… but if there is no package… uh… pkg++ probably failed.
		 */
		error "Building '$TARGET' failed."
		exit 1
	fi
}

build_needed() {
	/*
	 * build_needed() just checks if $TARGET exists and if it is up to 
	 * date.
	 */
	local FILE RESULT
	
	RESULT="yes"
	if [[ -f $TARGET ]]; then
		RESULT="no"
		for FILE in $PKGMK_PKGFILE ${source[@]}; do
			FILE=`get_filename $FILE`
			if [[ ! -e $FILE ]] || [[ ! $TARGET -nt $FILE ]]; then
				RESULT="yes"
				break
			fi
		done
	fi
	
	echo $RESULT
}

