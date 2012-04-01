
#<
# ## Default automatic splits module
# 
# ### Synopsis
# 
# This module contains the required functions for all default autosplits.
#>

#<
# ### Provided splits
# 
# #### `man`
# 
# The `man` autosplit puts all man pages into a `$pkgname-man` package.
#>
autosplits:man:splitable() {
	[[ -d "$PKG$mandir" ]]
}
autosplits:man:setsplit() {
	man_pkgname="${pkgname}-man"
	man_description="Manual pages for $name."
	man_class="man-pages"
}
autosplits:man:split() {
	pkgsplit man "$mandir"
}

#<
# #### `doc`
# 
# The `doc` autosplit is meant to put all documentation into a `$pkgname-doc`
# package.
#>
autosplits:doc:splitable() {
	[[ -d "$PKG$docdir" ]]
}
autosplits:doc:setsplit() {
	doc_pkgname="${pkgname}-doc"
	doc_description="Documentation for $name."
	doc_class="documentation"
}
autosplits:doc:split() {
	pkgsplit doc "$docdir"
}

#<
# #### `locale`
# 
# The `locales` split contains all locales and NLS-relative data into a 
# `$pkgname-locale` package.
# 
# The directories into which the locales are are `$localedir` and `$nlsdir`.
# `$nlsdir` can be defined in the recipe if the package possesses locales
# outside `$localedir`.
#>
autosplits:locale:splitable() {
	[[ -d "$PKG${localedir}" ]] || [[ -n "$nlsdir" && -d "$PKG$nlsdir" ]]
}
autosplits:locale:setsplit() {
	locale_pkgname="${pkgname}-locale"
	locale_description="Translations and internationalization files for ${name}"
	locale_class="locale"
}
autosplits:locale:split() {
	for dir in "${localedir}" "$nlsdir"; do
		if [[ -n "$dir" && -d "$PKG$dir" ]]; then
			pkgsplit locale "$dir"
		fi
	done
}

