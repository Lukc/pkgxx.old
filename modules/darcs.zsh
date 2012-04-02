
#<
# ## Darcs ##
# 
# The *darcs* module provides the required tools to use Darcs
# repositories as sources of recipes.
# 
# The module allows cloning and updating already cloned repositories.
# 
# The keyword to use to add monotone URLs in `${source[]}` is `darcs`.
# 
# ### Notes
# 
# Internally, we use the --lazy option to get repositories. Without
# this option, cloning any repository can become extraordinarily 
# long, as Darcs tends to be slow.
# 
#>

darcs:clone() {
	# Man, without --lazy, we’re screwed. Darcs’ so slooow.
	darcs get --lazy ${1#darcs+} $name
}

darcs:pull() {
	if [[ "`darcs pull`" =~ "No remote changes to pull in!" ]]; then
		return 1
	fi
}

