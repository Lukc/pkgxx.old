#<
# # Include for googlecode.com
# 
# The include `googlecode.com` allows easily exporting `$url`, 
# `${source[]}` and `$lastver` for a software hosted on Google Code.
# 
# ## Important variables
# 
#   * `$gc_name` is the project name on Google Code. It is also the
#     name of the tarball to use as source. Its default value is
#     `$name.`.
#   * `$gc_version` is the version of the software on Google Code.
#     Its default value is `$version`.
#   * `$gc_extension` is the extension of the tarball to fetch.
#     The default extension being `.tar.gz`.
#>

: ${gc_name:=$name}
: ${gc_version:=$version}
: ${gc_extension:=.tar.gz}

gc_url="http://code.google.com/p/$gc_name"
: ${url:="$gc_url"}

if [[ -z "${source[@]}" ]]; then
	source=("http://$gc_name.googlecode.com/files/$gc_name-$gc_version$gc_extension")
fi

: ${lastver:="lasttar 'http://code.google.com/p/${gc_name}/downloads/list' '$gc_name'"}

# vim: syntax=sh shiftwidth=4 tabstop=4
