
/*
 * What to say ? The three functions are used to display informations. They can
 * be redeclared in the config file.
 * Update [2011/02/11]: We broke compatibility with some Crux scripts by
 *   changing those functions. Compatibility had problems before because 
 *   of the colors which were not present in pkgmk, but now, nothing parsing
 *   the output of pkgmk should work with pkg++ if no configuration has 
 *   been done to avoid it. I [Lukc] hope this modification will open
 *   a new era of peace and prosperity on pkg++, and a little more fun
 *   to packaging.
 */

info() {
	echo -e "\033[01;32m--\033[00;01m $1\033[00m"
}

warning() {
	echo -e "\033[01;33m--\033[33;01m WARNING: $1\033[00m" >&2
}

error() {
	echo -e "\033[01;31m--\033[31;01m ERROR: $1\033[00m" >&2
}

/*
 * Use this *only* in interactive mode. Any other use could destroy the world.
 */

ask() {
	local variable="$1"
	shift 1
	echo -e "\033[01;36m--\033[00;01m $@\033[00m"
	echo -n " > "
	read "$variable"
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
