lprintf () {
	# `printf` `string` `[arguments]`
	
	local breakline=true
	
	if [[ "$1" == "-n" ]]; then
		breakline=false
		shift 1
	fi
	
	local str="$1"
	shift 1
	
	#       is a number in our strings. We shouldn’t need them, but
	#       if we do, we will need to rewrite the following six 
	if (( $# > 0 )); then
		for n in {1..$#}; do
			str="${str//\%$n/$1}"
			shift 1
		done
	fi
	
	while [[ "$str" =~ [^%]"%"[0-9]+ ]]; do
		# FIXME: THIS IS WRONG. It just does not work up to 9, to 
		#        begin with. Also, %%X is expected to cause trouble.
		str="${str/\%[0-9]}"
	done
	
	echo -n "$str"
	
	if $breakline; then
		echo
	fi
}

load_locales () {
	for LOCALE in en ${PKGMK_LOCALE%_*} ${PKGMK_LOCALE%.*} ${PKGMK_LOCALE}; do
		if [[ -e "$PKGMK_LOCALES_DIR/$LOCALE.sh" ]]; then
			. "$PKGMK_LOCALES_DIR/$LOCALE.sh"
		fi
	done
}

get_l10ned_var () {
	# 	$1: string
	# 	$2: locale
	# 	$3: element of table
	if [[ -z "$3" ]]; then
		eval "echo \${${1}_${2}}"
	else
		eval "echo \${${1}_${2}[${3}]}"
	fi
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
