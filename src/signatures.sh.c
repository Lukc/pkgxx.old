
sign_packages() {
	if [[ -n "$PKGMK_KEY" ]]; then
		for PKG in $(list_splits); do
			info "$msg_signing" "$PKG"
			gpg --detach-sign --use-agent -u ${PKGMK_KEY} "$filename"
		done
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
