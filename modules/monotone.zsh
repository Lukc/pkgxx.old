
#<
# ## Monotone ##
# 
# The *monotone* module provides the required tools to use monotone
# repositories as sources of recipes.
# 
# The module allows cloning, getting revisions and updating already cloned
# repositories.
# 
# The keyword to use to add monotone URLs in `${source[]}` is `mtn`.
# 
# ### Copyright ###
# 
# The Monotone module was a contribution from Pingax.
#>

mtn:clone() {
	mtn clone $1 $name
}

mtn:revision() {
	cd "$1"
	mtn automate log -q | wc -l 
}

mtn:pull() {
	if [[ "`mtn automate pull --dry-run -q | grep revision`" = "receive_revision "0"" ]]; then
		return 1
	fi
}

