make_control_sum() {
	local FILE LOCAL_FILENAMES
	local ALGORITHM="$1"
	
	if [[ -n "${source[@]}" ]]; then
		for FILE in ${source[@]}; do
			local LOCAL_FILE="$(get_filename "$FILE")"
			if [[ ! -d "$LOCAL_FILE" ]]; then
				LOCAL_FILENAMES=(${LOCAL_FILENAMES[@]} "$(get_filename "$FILE")")
			fi
		done
		
		[[ -n "$LOCAL_FILENAMES" && $(type ${ALGORITHM}:sum) != none ]] && \
			${ALGORITHM}:sum ${LOCAL_FILENAMES[*]} | sort -k 2
	fi
}

check_control_sums() {
	local FILE="$(mktemp "$PKGMK_WORK_DIR/tmp.XXXXXXXXXX")"
	local RETURN=0
	
	cd "$PKGMK_ROOT"

	for ALG in ${PKGMK_CONTROL_SUMS[@]}; do
		local FILE_ORIGINAL="$(get_metafile .${ALG})"

		if [[ ! -f "$FILE_ORIGINAL" ]]; then
			if [[ "$PKGMK_CHECK_CONTROL_SUMS" = "yes" ]]; then
				if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
					rm -rf $PKGMK_WORK_DIR
				fi
				
				info "$msg_algo_notfound" "${ALG}"
				RETURN=E_CONTROL_SUM
			fi
			
			warning "$msg_create_algo" "${ALG}"
			make_control_sum ${ALG} > ${FILE_ORIGINAL}
		else
			make_control_sum ${ALG} > ${FILE}
			sort -k 2 ${FILE_ORIGINAL} > ${FILE}.orig
			
			diff -w -t -U 0 ${FILE_ORIGINAL} ${FILE} | \
				sed '
					/^@@/d
					/^+++/d
					/^---/d
					s/^+/NEW       /g
					s/^-/MISSING   /g
				' > ${FILE}.diff
			
			if [[ -s ${FILE}.diff ]]; then
				error "$msg_algo_mismatch" "${ALG}"
				cat ${FILE}.diff >&2
				
				if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
					rm -rf $PKGMK_WORK_DIR
				fi
				
				if [[ "$PKGMK_CHECK_CONTROL_SUMS" = "yes" ]]; then
					error "$msg_algo_notok" "${ALG}"
					RETURN=E_CONTROL_SUM
				fi
				
				error "$msg_build_failed" "${TARGET}"
				RETURN=E_CONTROL_SUM
			fi
		fi
		
		if [[ "$PKGMK_CHECK_CONTROL_SUMS" = "yes" ]]; then
			info "$msg_algo_ok" "${ALG}"
		fi
	done
	
	if [[ "$RETURN" != 0 || "$PKGMK_CHECK_CONTROL_SUMS" = "yes" ]]; then
		if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
			rm -rf $PKGMK_WORK_DIR
		fi
		
		exit $RETURN
	fi
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
