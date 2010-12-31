
/* 
 * This module allows using a FrugalBuild instead of a Pkgfile.
 * Maybe a day, it could be used with Arch’s recipes…
 * 
 * WARNING: Using FrugalBuilds instead of Pkgfiles is very very very very very
 * very very very very very very very very very very unrecommended. (note the
 * line of “very”)
 * 
 * It depends on includes/frugalbuild.
 */

if [[ "$PKGMK_PKGFILE" = FrugalBuild ]]; then
	readonly includes=(frugalbuild)
	
	function strip_url {
		echo "$1" | sed 's|^.*:/\/.*/||g'
	}
	
	
	function Finclude {
		for i in $@; do
			. $PKGMK_ROOT/../../include/$i.sh
		done
	}
	Finclude util
fi

