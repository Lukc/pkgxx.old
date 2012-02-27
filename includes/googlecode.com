
: ${gc_name:=$name}
: ${gc_version:=$version}
: ${gc_extension:=.tar.gz}

gc_url="http://code.google.com/p/$gc_name"
: ${url:="$gc_url"}
if [[ -z "${source[@]}" ]]; then
	source=("http://$gc_name.googlecode.com/files/$gc_name-$gc_version$gc_extension")
fi
: ${lastver:="lasttar 'http://code.google.com/p/${gc_name}/downloads/list' '$gc_name'"}

