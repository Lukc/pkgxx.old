ask_use() {
	# Function allowing to dynamically change a use flag in interactive
	# mode.
	local ANSWER=
	local use="$1"
	local desc="$(get_use_description "${use}")"
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
					info "$msg_no_userflag_info" "${1}"
				fi
			;;
			*)
				error "Please, answer by \`yes', \`y', \`no' or \`n'."
			;;
		esac
	done
}

interact_uses() {
	if [[ "$PKGMK_INTERACTIVE" = "yes" ]]; then
		for FLAG in ${iuse[@]}; do
			ask_use "$FLAG"
		done
	fi
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
