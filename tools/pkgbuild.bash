#!/usr/bin/env bash

# FIXME: Packagers, builders, contributors…
# FIXME: Automated tools. (install=, for example)
# FIXME: Integrated md5sums…

. "$1"

cat "$1"

echo "name=$pkgname"
echo "version=$pkgver"
echo "release=$pkgrel"
echo "description='$pkgdesc'"
echo -n "source=( "
for i in ${source[@]}; do
	case $i in
		*://*) echo -n "$i " ;;
		*) echo -n "file://$i " ;;
	esac
done
echo ")"
echo

if [[ -n "$(type package 2> /dev/null)" ]]; then
	echo "post_build() {"
	# Too lazy to do something that will do the job correctly.
	type package | egrep -v "^package is a function$"
	echo "}"
fi

# This is here to prevent the PKGBUILD from not having it’s own $PKG, $WORK, etc.
echo "startdir=\$PKGMK_WORK_DIR"
echo "srcdir=\$startdir/src"
echo "pkgdir=\$startdir/pkg"



