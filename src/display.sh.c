
/*
 * What to say ? The three functions are used to display informations. They can
 * be redeclared in the config file.
 */

info() {
	echo -e "\033[32m=======> $1\033[00m"
}

warning() {
	echo -e "\033[33m=======> WARNING: $1\033[00m" >&2
}

error() {
	echo -e "\033[31m=======> ERROR: $1\033[00m" >&2
}

/*
 * Use this *only* in interactive mode. Any other use could destroy the world.
 */

ask() {
	local variable="$1"
	shift 1
	echo -e "\033[37m=======> $@\033[00m"
	echo -n " > "
	read "$variable"
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
