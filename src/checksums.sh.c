
make_md5sum() {
	/*
	 * MD5 sucks, but is here for Crux’s compatibility.
	 */
	local FILE LOCAL_FILENAMES
	
	if [[ -n "${source[@]}" ]]; then
		for FILE in ${source[@]}; do
			if [[ ! -d "`get_filename $FILE`" ]]; then
				LOCAL_FILENAMES=(${LOCAL_FILENAMES[@]} `get_filename $FILE`)
			fi
		done

		[[ -n "$LOCAL_FILENAMES" ]] && (
			if [[ "$md5sum_gnu" = "true" ]]; then
				md5sum ${LOCAL_FILENAMES[@]} | sed -e 's|  .*/|  |' | sort -k 2
			else
				for FILE in $LOCAL_FILENAMES; do
					local md5=$(md5 $FILE | sed -e 's|  .*|  |;s/MD5 (.*) = //' | sort -k 2)
					echo "$md5  $(basename $FILE)"
				done
			fi
		)
	fi
}

make_sha256sum() {
	/*
	 * SHA256 is much better, but few ports already uses it.
	 */
	local FILE LOCAL_FILENAMES

	if [[ -n "${source[@]}" ]]; then
		for FILE in ${source[@]}; do
			if [[ ! -d "`get_filename $FILE`" ]]; then
				LOCAL_FILENAMES=(${LOCAL_FILENAMES[@]} `get_filename $FILE`)
			fi
		done
		
		[[ -n "$LOCAL_FILENAMES" ]] && (
			if [[ "$sha256sum_gnu" = "true" ]]; then
				sha256sum ${LOCAL_FILENAMES[@]} | sed -e 's|  .*/|  |' | sort -k 2
			else
				for FILE in $LOCAL_FILENAMES; do
					local sha256=$(sha256 $FILE | sed -e 's|  .*|  |;s/SHA256 (.*) = //' | sort -k 2)
					echo "$sha256  $(basename $FILE)"
				done
			fi
		)
	fi
}

check_md5sum() {
	/*
	 * We compare the new md5sums to the old ones. If there is a 
	 * difference, then we print an error.
	 */
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
				exit E_CONTROL_SUM
			fi

			error "Building '$TARGET' failed."
			exit E_CONTROL_SUM
		fi
	else
		if [[ "$PKGMK_CHECK_MD5SUM" = "yes" ]]; then
			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi
			info "Md5sum not found."
			exit E_CONTROL_SUM
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
	/*
	 * Same thing, for SHA256.
	 */
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
				exit E_CONTROL_SUM
			fi

			error "Building '$TARGET' failed."
			exit E_CONTROL_SUM
		fi
	else
		if [[ "$PKGMK_CHECK_SHA256SUM" = "yes" ]]; then
			if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
				rm -rf $PKGMK_WORK_DIR
			fi
			info "Sha256sum not found."
			exit E_CONTROL_SUM
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

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
