
#<
# # debian.zsh
# 
# This module provides classes for Debian.
# 
#>

Debian:library() {
	#<
	# ## `Debian:library`
	# 
	# Adds `lib` at the begining of `$pkgname`, if it does not already
	# begins by the same string.
	# 
	#   # Examples
	#   tre    -> libtre
	#   libsdl -> libsdl
	#>
	pkgname="lib${pkgname}"
	pkgname="${pkgname/^liblib/lib}"
}

Debian:documentation() {
	#<
	# ## `Debian:documentation`
	# 
	# Adds `-doc` at the end of `$pkgname` and removes duplicate `-doc`.
	#>
	pkgname="${pkgname}-doc"
	pkgname="${pkgname/-doc-doc$/-doc}"
}

Debian:headers() {
	#<
	# ## `Debian:headers`
	# 
	# Adds `-dev` at the end of `$pkgname` and removes duplicate `-dev`.
	#>
	pkgname="${pkgname}-dev"
	pkgname="${pkgname/-dev-dev$/-dev}"
}

