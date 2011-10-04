
file:copy() {
	cp "$PKGMK_ROOT/${1/file:\/\//}" "$PKGMK_SOURCE_DIR/$(basename "$1")"
}

