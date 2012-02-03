
#<
# ## Mercurial ##
# 
# The Mercurial module allows you to use Mercurial repositories as
# sources in your recipes.
#
# It supports cloning, updating already cloned repositories and 
# getting revision numbers.
# 
# Please, do not use it with `git://` URLs. If you want to use `hg`
# to clone git repositories, edit the git module.
#
# The keyword of this module is `hg`.
# 
#>

hg:clone() {
	hg clone `echo $1 | sed -e 's|^hg+||'` $name
}

hg:revision() {
	cd "$1"
	echo $(hg tip | head -n 1 | cut -d : -f 2)
}

hg:pull() {
	if [[ "`hg pull | tail -n 1`" = "no changes found" ]]; then
		return 1
	else
		hg update > /dev/null
	fi
}

