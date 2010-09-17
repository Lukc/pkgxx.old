
make_md5sum() {
	local FILE LOCAL_FILENAMES
	
	if [[ "$source" ]]; then
		for FILE in ${source[@]}; do
			if [[ ! -d "`get_filename $FILE`" ]]; then
				LOCAL_FILENAMES="$LOCAL_FILENAMES `get_filename $FILE`"
			fi
		done
		
		[[ -n "$LOCAL_FILENAMES" ]] && md5sum $LOCAL_FILENAMES | sed -e 's|  .*/|  |' | sort -k 2
	fi
}

make_sha256sum() {
	local FILE LOCAL_FILENAMES

	if [[ "$source" ]]; then
		for FILE in ${source[@]}; do
			if [[ ! -d "`get_filename $FILE`" ]]; then
				LOCAL_FILENAMES="$LOCAL_FILENAMES `get_filename $FILE`"
			fi
		done
		
		[[ -n "$LOCAL_FILENAMES" ]] && sha256sum $LOCAL_FILENAMES | sed -e 's|  .*/|  |' | sort -k 2
	fi
}

check_md5sum() {
	local FILE="$PKGMK_WORK_DIR/.tmp"

	cd $PKGMK_ROOT
	
	if [[ -f $PKGMK_MD5SUM ]]; then
		make_md5sum > $FILE.md5sum
		sort -k 2 $PKGMK_MD5SUM > $FILE.md5sum.orig
		diff -w -t -U 0 $FILE.md5sum.orig $FILE.md5sum | \
			sed '/^@@/d' | \
			sed '/^+++/d' | \
			sed '/^---/d' | \
			sed 's/^+/NEW       /g' | \
			sed 's/^-/MISSING   /g' > $FILE.md5sum.diff
		if [[ -s $FILE.md5sum.diff ]]; then
			error "Md5sum mismatch found:"
			cat $FILE.md5sum.diff >&2

			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi

			if [[ "$PKGMK_CHECK_MD5SUM" = "yes" ]]; then
				error "Md5sum not ok."
				exit $E_CONTROL_SUM
			fi

			error "Building '$TARGET' failed."
			exit $E_CONTROL_SUM
		fi
	else
		if [[ "$PKGMK_CHECK_MD5SUM" = "yes" ]]; then
			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi
			info "Md5sum not found."
			exit $E_CONTROL_SUM
		fi
		
		warning "Md5sum not found, creating new."
		make_md5sum > $PKGMK_MD5SUM
	fi

	if [[ "$PKGMK_CHECK_MD5SUM" = "yes" ]]; then
		if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
			rm -rf $PKGMK_WORK_DIR
		fi
		info "Md5sum ok."
		exit 0
	fi
}

check_sha256sum() {
	local FILE="$PKGMK_WORK_DIR/.tmp"

	cd $PKGMK_ROOT
	
	if [[ -f $PKGMK_SHA256SUM ]]; then
		make_sha256sum > $FILE.sha256sum
		sort -k 2 $PKGMK_SHA256SUM > $FILE.sha256sum.orig
		diff -w -t -U 0 $FILE.sha256sum.orig $FILE.sha256sum | \
			sed '/^@@/d' | \
			sed '/^+++/d' | \
			sed '/^---/d' | \
			sed 's/^+/NEW       /g' | \
			sed 's/^-/MISSING   /g' > $FILE.sha256sum.diff
		if [[ -s $FILE.sha256sum.diff ]]; then
			error "Sha256sum mismatch found:"
			cat $FILE.sha256sum.diff >&2

			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi

			if [[ "$PKGMK_CHECK_SHA256SUM" = "yes" ]]; then
				error "Sha256sum not ok."
				exit $E_CONTROL_SUM
			fi

			error "Building '$TARGET' failed."
			exit $E_CONTROL_SUM
		fi
	else
		if [[ "$PKGMK_CHECK_SHA256SUM" = "yes" ]]; then
			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi
			info "Sha256sum not found."
			exit $E_CONTROL_SUM
		fi
		
		warning "Sha256sum not found, creating new."
		make_sha256sum > $PKGMK_SHA256SUM
	fi

	if [[ "$PKGMK_CHECK_MD5SUM" = "yes" ]]; then
		if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
			rm -rf $PKGMK_WORK_DIR
		fi
		info "Md5sum ok."
		exit 0
	fi
}
