
# This module allows using a FrugalBuild instead of a Pkgfile.
# Maybe a day, it could be used with Arch’s recipes…
# 
# WARNING: Using FrugalBuilds instead of Pkgfiles is very very very very very
# very very very very very very very very very very unrecommended. (note the
# line of “very”)
# 
# It depends on includes/frugalbuild and has effect only with FrugalBuilds.

if [[ "$PKGMK_PKGFILE" =~ (.*/|)FrugalBuild ]]; then
	includes=(frugalbuild)
	
	function strip_url {
		echo "$1" | sed 's|^.*:/\/.*/||g'
	}
	
	# The heart of FrugalBuilds.
	function Finclude {
		for i in $@; do
			. $PKGMK_ROOT/$(dirname "$PKGMK_PKGFILE")/../../include/$i.sh
		done
	}
	
	# I/O functions
	function msg {
		echo -e "\033[00;34m   > \033[00m$@"
	}
	
	function Fmessage {
		echo -e "\033[00;34m   > \033[00m$@"
	}
	
	# Many many things used for potentially anything in recipes…
	Finclude util
	# Even if you don’t want it, it’ needed… as FrugalBuilds are 
	# made to work with a packages builder which has this behavior.
	PKGMK_NOFAIL="yes"
fi

