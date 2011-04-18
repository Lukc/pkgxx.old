
/* 
 * Use this before split_exec.
 */
#define export_splits \
	PKG_NAME="$name" ; \
	PKG_NAMES=("$name" ${splits[@]}) ; \
	PKG_VERSIONS=("$version" ${splits_versions[@]}) ; \
	PKG_LICENSES=("$license" ${splits_licenses[@]}) ; \
	PKG_DESC=("$description" ${splits_descriptions[@]}) ; \
	PKG_ARCHS=("${archs[@]}" ${splits_archs[@]}) ; \
	PKG_DEPENDS=("${depends[@]}" ${splits_depends[@]})
/* For the depends, an array of strings that contain the original arrays… */

/* 
 * Internal, here only for split_exec.
 */
#define split_get_scriptname(SCRIPT) \
	"$(if [[ "${PKG_NAME}" = "${PKG_NAMES[$i]}" ]]; then \
		if [[ -e "${PKGMK_ROOT}/${SCRIPT}" ]]; then \
			cat "${PKGMK_ROOT}/${SCRIPT}"; \
		fi; \
	else \
		if [[ -e "${PKGMK_ROOT}/${PKG_NAMES[$i]}.${SCRIPT}" ]]; then \
			cat "${PKGMK_ROOT}/${PKG_NAMES[$i]}.${SCRIPT}"; \
		fi; \
	fi)"

/* 
 * Usage: split_exec command opts
 */
#define split_exec \
	name="${PKG_NAMES[$i]}" version="${PKG_VERSIONS[$i]:-$version}" \
	license="${PKG_LICENSES[$i]:-$license}" \
	description="${PKG_DESC[$i]}" archs=(${PKG_ARCHS[$i]}) \
	depends=(${PKG_DEPENDS[$i]}) PKG="$PKG_ROOT" \
	TARGET="$(get_target)" \
	PRE_INSTALL=split_get_scriptname(PKGMK_PRE_INSTALL) \
	POST_INSTALL=split_get_scriptname(PKGMK_POST_INSTALL) \
	PRE_REMOVE=split_get_scriptname(PKGMK_PRE_REMOVE) \
	POST_REMOVE=split_get_scriptname(PKGMK_POST_REMOVE)

