
#<
# ## Bazaar ##
# 
# The Bazaar module (or `bzr`) allows you to use bazaar repositories
# as sources. If your URL does not begin with `bzr://`, do not forget
# to prefix it with `bzr+`.
# 
# The module allows cloning and updating already cloned repositories.
# 
# The keyword of this module is `bzr`.
#>

bzr:clone() {
	bzr branch `echo $1 | sed -e 's|^bzr+||'` $name
}

bzr:pull() {
	if [[ "`bzr update 2>&1`" =~ "Tree is up to date at revision ".*" of branch ".* ]]; then
		return 1
	fi
}

