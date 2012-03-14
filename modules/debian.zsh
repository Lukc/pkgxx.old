
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
	#   # Exemples
	#   tre    -> libtre
	#   libsdl -> sdl
	#>
	pkgname="lib${pkgname}"
	pkgname="${pkgname/^liblib/lib}"
}

