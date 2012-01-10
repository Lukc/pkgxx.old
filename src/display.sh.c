
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
	echo -e "${fg_bold[green]}--${fg_bold[white]} $1${reset_color}"
}

warning() {
	echo -e "${fg_bold[yellow]}-- WARNING: $1${reset_color}" >&2
}

error() {
	echo -e "${fg_bold[red]}-- ERROR: $1${reset_color}" >&2
}

/* 
 * This may be used to print messages from inside build(), check() and other
 * recipe-defined functions, even if internal-use is also planed.
 */

subinfo() {
	echo "${fg_bold[blue]} --${fg_bold[white]} $@${reset_color}"
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
