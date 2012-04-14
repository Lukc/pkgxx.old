
lprintf () {
	/* `printf` `string` `[arguments]` */
	
	local breakline=true
	
	if [[ "$1" == "-n" ]]; then
		breakline=false
		shift 1
	fi
	
	local str="$1"
	shift 1
	
	/* Note: The following lines do not allow us to use %x where x
	 *       is a number in our strings. We shouldn’t need them, but
	 *       if we do, we will need to rewrite the following six 
	 *       lines, and make "%%" or "\%" appear like a simple "%". */
	if (( $# > 0 )); then
		for n in {1..$#}; do
			str="${str//\%$n/$1}"
			shift 1
		done
	fi
	
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

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
