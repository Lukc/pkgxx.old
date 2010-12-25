
pkg_manager_add(rpm)

rpm:unpack() {
	info "Unpacking $1."
	case "$PKGMK_UNRPM_TOOL" in
		bsdtar)
			bsdtar -p -o -C $2 -xf $1
		;;
		rpm)
			error "Hey, we don't already support that!"
			error "If you can help, please, send a patch."
			exit E_GENERAL
		;;
	esac
}

/* 
 * Note : There is no rpm:pack(), as rpm:build() already exists and should
 *        be used.
 */

rpm:list() {
	/* 
	 * FIXME
	 */
	:
}

