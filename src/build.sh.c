
build_package() {
	local BUILD_SUCCESSFUL="no"
	
	check_file "$TARGET"
	make_work_dir
	
	if [[ "$UID" != "0" ]]; then
		warning "Packages should be built as root."
	fi
	
	info "Building '$TARGET'."
	
	unpack_source
	
	cd $SRC
	if [[ "`type -t pre_build`" = "function" ]]; then
		(set -e -x ; pre_build)
	fi
	(set -e -x ; build)
	if [[ "`type -t post_build`" = "function" ]]; then
		(set -e -x ; post_build)
	fi
	
	if [[ "$PKGMK_CHECK" = "yes" ]]; then
		info "Testing $TARGET."
		if check; then
			info "$TARGET successfully tested."
		else
			error "Tests of $TARGET failed."
			exit 1
		fi
	fi
	
	if [[ $? = 0 ]]; then
		if [[ "$PKGMK_NO_STRIP" = "no" ]]; then
			strip_files
		fi
		
		compress_manpages
		
		cd $PKG
		if [[ "`find . | wc -l`" = 1 ]]; then
			error "Building '$TARGET' failed."
			exit 1
		fi
		info "Build result:"
		#if defined dpkg
		mkdir DEBIAN
		[[ -e DEBIAN/control ]] && rm DEBIAN/control
		echo "Package: $name" >> DEBIAN/control
		echo "Version: $version" >> DEBIAN/control
		if [[ "$arch" = "no-arch" ]] || [[ "$kernel" = "no-kernel" ]]; then
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
		make_rpm_spec > $PKGMK_WORK_DIR/$name.spec
		rpmbuild --define "_topdir $PKGMK_PACKAGE_DIR/RPM" --quiet --buildroot=$PKG -bb $PKGMK_WORK_DIR/$name.spec
		mv $PKGMK_PACKAGE_DIR/RPM/RPMS/$PKGMK_ARCH/$name-$version-$release.$PKGMK_ARCH.rpm $TARGET
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
			size="`du -cb . | tail -n 1 | awk '{print $1}'`"
			find . | sed "s|\./||" | sort > .FILELIST
			make_pacman_pkginfo > .PKGINFO
			unset size
			// FIXME: What about the Changelog ? :/
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
		mv ${TARGET%.$EXT}.$PKGMK_COMPRESSION_MODE ${TARGET}
		#endif
		#endif
		
		if [[ $? = 0 ]]; then
			BUILD_SUCCESSFUL="yes"
			
			if [[ "$PKGMK_IGNORE_FOOTPRINT" = "yes" ]]; then
				warning "Footprint ignored."
			else
				check_footprint
			fi
		fi
	fi
	
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		remove_work_dir
	fi
	
	if [[ "$BUILD_SUCCESSFUL" = "yes" ]]; then
		info "Building '$TARGET' succeeded."
	else
		if [[ -f $TARGET ]]; then
			touch -r $PKGMK_ROOT/$PKGMK_PKGFILE $TARGET &> /dev/null
		fi
		error "Building '$TARGET' failed."
		exit 1
	fi
}

build_needed() {
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

