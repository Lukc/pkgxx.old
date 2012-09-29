
require () {
	# This might be usefull in the future, if libpkgxx is meant
	# to be used for more than extracting Pkgfiles’s data. (or
	# for other reasons)
	while [[ -n "$1" ]]; do
		. @LIBDIR@/pkgxx/$1.zsh
		shift 1
	done
}

source () {
	# The implementations of source() are not standard, and calling "." 
	# may not have the expected result on zsh. This source() should be
	# portable and avoid problems between shells.
	local FILE="$1"
	shift 1
	if [[ "$(dirname "$FILE")" = "." ]]; then
		. "./$FILE" $@
	else
		. "$FILE" $@
	fi
}

# FIXME: Portability: bash, ash, maybe others… This has to be rewritten
#        for each shell, or almost.
type() {
	# Each shell has its own type(). This one is currently working only
	# on zsh.
	# It returns the type of the function/executable/… given in $1.
	builtin type -w "$1" | sed -e "s/${1//\//\\/}: //"
}

if [[ "$(type tac)" == "none" ]]; then
	tac() {
		# tac() is not present in non-GNU non-busybox userlands. This 
		# should solve any problem encountered.
		nl -ba $@ | sort -nr | cut -f2-
	}
fi

which() {
	# Same problem as for type(). This implementation of which solves 
	# incompatibilities due to different shells or userlands.
	local TARGET="$1"

	for path in ${PATH//:/ }; do
		if [[ -x "$path/$TARGET" ]]; then
			echo "$path/$TARGET"
			return 0
		fi
	done

	echo "$TARGET"
	return 1
}

use() {
	# Very important function that returns true(0) if a use flag is 
	# used, and false(1) if it is not. It also returns 3 if the flag
	# is not in the user’s configuration and won’t be used.
	local return=2
	for flag in ${USE[@]}; do
		if [[ "$flag" = "$1" ]] || [[ "$flag" = "+$1" ]]; then
			return=0
		elif [[ "$flag" = "-$1" ]]; then
			return=1
		fi
	done

	if (( $return == 2 )); then
		local eval=$(eval "echo \${use_${1}[1]}")
		if [[ "$eval" = 1 ]]; then
			eval=0
		elif [[ "$eval" = 0 ]]; then
			# Do not use because not specified otherwise anywhere
			eval=3
		fi
		if [[ -n "$eval" ]]; then
			return $eval
		fi
	fi

	return $return
}

# FIXME: vercmp() may not work as it is outside zsh.
vercmp () {
	local comp=$2
	local i=1
	local version1=$(echo "$1" | sed -e "s/-/./g;s/rc\([0-9]\)/.rc.\1/g;s/\.\././g").0
	local version2=$(echo "$3" | sed -e "s/-/./;s/rc\([0-9]\)/.rc.\1/g;s/\.\././g")
	
	# Traditional sh comparison operators should be supported, because
	# they don’t require to be quoted
	case "$comp" in
		-gt)
			comp=">"
		;;
		-ge)
			comp=">="
		;;
		-lt)
			comp="<"
		;;
		-le)
			comp="<="
		;;
		-eq)
			comp="=="
		;;
		-ne)
			comp="!="
		;;
		-sb)
			# “-sb” stands for Same Branch.
			comp="~>"
		;;
	esac
	
	if [[ "$comp" != "~>" ]]; then
		version2="$version2".0
	fi
	local v1=$(echo "$version1" | cut -d '.' -f $i)
	local v2=$(echo "$version2" | cut -d '.' -f $i)
	
	alpha=-3
	beta=-2
	rc=-1
	devel=99999 # If you ever find a version greater than this, update
	while [[ -n "$v1" && -n "$v2" ]]; do
		if ! (( $v1 == $v2 )); then
			if [[ "$comp" =~ ("!="|"~=") ]]; then
				return 0
			elif [[ "$comp" == "~>" ]]; then
				return 1
			else
				(( $v1 $comp $v2 ))
				return $?
			fi
		fi
		i=$(($i+1))
		v1=$(echo "$version1" | cut -d "." -f $i)
		v2=$(echo "$version2" | cut -d "." -f $i)
		[[ -z "$v2" && "$comp" == "~>" ]] && return 0
		[[ -n "$v1" && -z "$v2" ]] && v2=0
		[[ -n "$v2" && -z "$v1" ]] && v1=0
	done
	# If everything was equal.
	[[ "$comp" =~ ("!="|"~="|">"|"<") ]] && return 1 || return 0
}

@{ () {
	case $# in
		0|1|2)
			error "@{ needs 3 parameters and to be closed."
			return 1
		;;
		3)
			error "@{ needs to be closed."
			return 1
		;;
		4);;
		*)
			error "@{ needs only 3 parameters and to be closed."
			return 1
		;;
	esac
	# This is debug to avoid having problems with vercmp.
	echo "$1$2$3" | egrep -q "[ 	]" && {
			error "@{: opts must not contain any space or tabulation."
			return 1
		}
	for i in 1 2 3; do
		eval "[[ -z \$$i ]]" && {
			error "@{: empty opt (\$$i)."
			return 1
		}
	done
	vercmp "$1" "$2" "$3"
}

depname() {
	# Extracts the name of the package from a string looking like
	# "pkg comp ver", where comp is a versions comparison operator.
	echo "$1" | sed -e "s/ .*//;s/(>|<|>=|<=|=|==|~>|~<|!=|~=).*//"
}

