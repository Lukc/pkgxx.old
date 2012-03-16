
#<
# # debian.zsh
# 
# This module provides classes and slots management for Debian.
# 
#>

Debian:slots() {
	#<
	# ## `Debian:slots`
	# 
	# Modifies `$split_pkgname` to add the `$deb_split_separator` and 
	# `$split_slot` to its end.
	# 
	# `$deb_split_separator` has no default value (is empty).
	#>
	: ${deb_split_separator:=""}
	split_pkgname="${split_pkgname}${deb_split_separator}${split_slot}"
}

Debian:class:library() {
	#<
	# ## `Debian:class:library`
	# 
	# Adds `lib` at the begining of `$split_pkgname`, if it does not already
	# begins by the same string.
	# 
	#   # Examples
	#   tre    -> libtre
	#   libsdl -> libsdl
	#>
	split_pkgname="lib${split_pkgname}"
	split_pkgname="${split_pkgname/^liblib/lib}"
}

Debian:class:documentation() {
	#<
	# ## `Debian:class:documentation`
	# 
	# Adds `-doc` at the end of `$split_pkgname` and removes duplicate `-doc`.
	#>
	split_pkgname="${split_pkgname}-doc"
	split_pkgname="${split_pkgname/-doc-doc$/-doc}"
}

Debian:class:headers() {
	#<
	# ## `Debian:class:headers`
	# 
	# Adds `-dev` at the end of `$split_pkgname` and removes duplicate `-dev`.
	#>
	split_pkgname="${split_pkgname}-dev"
	split_pkgname="${split_pkgname/-dev-dev$/-dev}"
}

