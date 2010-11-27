
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
	GMAKE $MAKE_OPTS $@
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

ask_use() {
	/*
	 * Function allowing to change a use flag in interactive mode.
	 */
	local ANSWER=
	local use="$1"
	local desc="$(get_use_desc "$use")"
	while [[ ! "$ANSWER" =~ (YES|yes|Y|y|NO|no|N|n) ]]; do
		ask ANSWER "[$name] Do you want to enable use flag \`$use'? [y/n/?]"
			case $ANSWER in
			YES|yes|Y|y)
				USE=(${USE[@]} +$use)
			;;
			NO|no|N|n)
				USE=(${USE[@]} -$use)
			;;
			\?)
				if [[ -n "$desc" ]]; then
					info "$use - $desc"
				else
					info "There is no available information about use flag \`$1'"
				fi
			;;
			*)
				error "Please, answer by \`yes', \`y', \`no' or \`n'."
			;;
		esac
	done
}

uses() {
	if [[ "$PKGMK_INTERACTIVE" = "yes" ]]; then
		for flag in $@; do
			ask_use $flag
		done
	fi
}
