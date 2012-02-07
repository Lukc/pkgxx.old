
: ${gc_name:=$name}
: ${gc_version:=$version}
: ${url:="http://code.google.com/p/$gc_name"}
: ${lastver:="lasttar 'http://code.google.com/p/${gc_name}/downloads/list' '$gc_name'"}

source=(${source[@]} http://$gc_name.googlecode.com/files/$gc_name-$gc_version.tar.gz)

