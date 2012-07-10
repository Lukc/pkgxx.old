#!/usr/bin/env sh

VERBOSE=false

# First, we need to parse the opts.
while [ -n "$1" ]; do
	case "$1" in
		-pkg)
			PKG="$2"
			shift 1
		;;
		-root)
			ROOT="$2"
			shift 1
		;;
		-verbose)
			VERBOSE=true
		;;
		*)
			echo "Unknown option \`$1'." >&2
		;;
	esac
	shift 1
done

# We check if $PKG and $ROOT are set…
# Note: PKG and ROOT may have been set by exporting env vars.
if [ -z "$PKG" ]; then
	echo "-pkg <package> is needed." >&2
	exit 1
fi
if [ -z "$ROOT" ]; then
	echo "-root <package-root> is needed." >&2
	exit 1
fi

cd $ROOT
if ! [ -e $ROOT/OPK/control ]; then
	echo "No $ROOT/OPK/control file found. Abording." >&2
	exit 1
fi

FILES=
for file in $ROOT/OPK/{control,preinst,postinst}; do
	if [ -e $file ]; then
		FILES="$FILES $file"
	fi
done

# 
### Composition of a .opk file:
# 
# package.opk                  (ar)
#  |- debian-binary            (text)
#  |- control.tar.gz           (tarball)
#  |   |- control              (YAML-like)
#  |   |- preinst              (script)
#  |   |- postinst             (script)
#  |   |- prerm                (script)
#  |   |- postrm               (script)
#  |- data.tar.gz              (tarball)
#      |- usr
#          |- bin
#              |- opkg-hello   (example program)

mkdir -p /tmp/opkgmk-$$
cd /tmp/opkgmk-$$
tar czf control.tar.gz $FILES
(
	cd $ROOT
	tar cz`$VERBOSE && echo -n vv`f /tmp/opkgmk-$$/data.tar.gz --exclude='./OPK/*' --exclude='./OPK' .
)
echo "2.0" > debian-binary
ar -r $PKG debian-binary data.tar.gz control.tar.gz

