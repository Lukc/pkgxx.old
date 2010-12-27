
/* 
 * Use this before split_exec.
 */
#define export_split \
	PKG_NAME="$name" ; \
	PKG_NAMES=("$name" "${splits[@]}") ; \
	PKG_VERSIONS=("$version" "${splits_version[@]}") ; \
	PKG_LICENSES=("$license" "${splits_licenses[@]}") ; \
	PKG_DESC=("$description" "${splits_descriptions[@]}") ; \
	PKG_ARCHS=("${archs[0]}" "${splits_archs[@]}") ; \
	PKG_DEPENDS=("${depends[@]}" "${splits_depends[@]}")

/* 
 * Usage: split_exec command opts
 */
#define split_exec \
	name="${PKG_NAMES[$i]}" version="${PKG_VERSIONS[$i]:-$version}" \
	license="${PKG_LICENSES[$i]:-$license}" \
	description="${PKG_DESC[$i]}" archs=(${PKG_ARCHS[$i]}) \
	depends="${PKG_DEPENDS[$i]}" PKG="$PKG_ROOT" \
	TARGET="$(get_target)"

