
: ${sysconfdir:="/etc"}
: ${localstatedir:="/var"}
: ${prefix:="/usr"}
: ${eprefix:="$prefix"}
: ${bindir:="$eprefix/bin"}
: ${sbindir:="$eprefix/sbin"}
: ${libdir:="$prefix/lib"}
: ${includedir:="$prefix/include"}
: ${libexecdir:="$eprefix/libexec"}
: ${sharedir:="$prefix/share"}
: ${datadir:="$prefix/share"}
: ${localedir:="$sharedir/locale"}
: ${docdir:="$sharedir/doc/$name-$version"}
if $Crux || $Slackware; then
	: ${mandir:="$prefix/man"}
else
	: ${mandir:="$sharedir/man"}
fi

# PKGMK_HOMEPREFIX should be a boolean, and is independant of $prefix.
if istrue $PKGMK_HOMEPREFIX; then
	# Those three variables are modified to allow building with
	#+ libraries and headers in alternative directories.
	export CFLAGS="-I$includedir $CFLAGS"
	export CXXFLAGS="-I$includedir $CXXFLAGS"
	export LDFLAGS="-L$libdir $LDFLAGS"
	
	if [[ -z "$LD_LIBRARY_PATH" ]]; then
		export LD_LIBRARY_PATH="$libdir"
	else
		export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$libdir"
	fi
fi

