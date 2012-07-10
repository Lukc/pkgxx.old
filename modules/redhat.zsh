
#<
# ## redhat.zsh
# 
# This module provides classes and slots management for the distributions 
# based on Red Hat and/or using RPM.
# 
# The list of distributions concerned is the following.
#
#   * Fedora
#   * Mandriva
#   * Mageia
#   * SuSE
#   * OpenSUSE
#>

#<
# ### Classes
#>

redhat:class:headers() {
	#<
	# #### `redhat:class:headers`
	# 
	# Adds `-devel` at the end of `$split_pkgname` and removes duplicate `-devel`.
	#>
	split_pkgname="${split_pkgname}-devel"
	split_pkgname="${split_pkgname/-devel-devel$/-devel}"
}

for D in Fedora Mandriva Mageia SuSE OpenSUSE; do
	alias ${D}:class:headers=redhat:class:headers
done
