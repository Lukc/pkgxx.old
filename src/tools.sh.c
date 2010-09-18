
has() {
	/*
	 * If $1 is in the other parameters, then returns true. Else returns 
	 * false.
	 */
	item="$1"
	shift 1
	for t_item in $@; do
		if [[ "$item" = "$t_item" ]]; then
			return 0
		fi
	done
	return 1
}

pkgmake() {
	/*
	 * That avoids to use a non-gnu make, which could cause problems on 
	 * some ports.
	 */
	@GMAKE@ $MAKE_OPTS $@
}

use() {
	/*
	 * Very important function that returns true if a use flag is used, and
	 * false if it is not.
	 */
	local return=1
	for flag in ${USE[@]}; do
		if [[ "$flag" = "$1" ]] || [[ "$flag" = "+$1" ]]; then
			return=0
		elif [[ "$flag" = "-$1" ]]; then
			return=1
		fi
	done
	return $return
}

