
#<
# ## Subversion ##
# 
# The Subversion module allows you to use Subversion repositories as
# sources in your recipes.
#
# It supports cloning and updating already cloned repositories.
#
# The keyword with which you can prefix your Subversion URLs is `svn`.
#>

SVN_OPTS=(--non-interactive --trust-server-cert)

svn:clone() {
	svn co ${SVN_OPTS[*]} `echo "$1" | sed -e 's|^svn+||'` $name
}

svn:pull() {
	# No need to set the LANG, as it is already exported in src/main.sh.c.
	if [[ "`svn up $SVN_OPTS`" =~ 'At revision '*'.' ]]; then
		return 1
	fi
}

