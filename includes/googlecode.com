
: ${gc_name:=$name}
: ${gc_version:=$version}
: ${url:="http://code.google.com/p/$gc_name"}
: ${source:=($url/files/$gc_name-$gc_version.tar.gz)}
: ${lastver:="lasttar 'http://code.google.com/p/${gc_name}/downloads/list' '$gc_name'"}

