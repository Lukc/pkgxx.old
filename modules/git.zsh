
#<
# ## git ##
# 
# The git module provides the required tools to fetch sources from git
# repositories, to get a revision number of a repository using the list
# of commits (to version numerically packages), to update an already
# cloned repository and to check if the last is up to date.
# 
# The keyword with which you can prefix git URLs is `git`.
#  
# For more information about how to use this module, please refer
# to the [modules reference](/doc/modules.xhtml).
#>

#<
# ### Important variables
# 
# #### `$git_branch`
# 
# Contains the name of the branch to be checked out. The default value is
# `master`.
#>
# master is the default, default branch.
: ${git_branch:=master}

#<
# ### Functions
#>

git:clone() {
	#<
	# #### `git:clone()`
	# 
	# Clones the git repository. Ignore SSL warnings and takes care of
	# `$git_branch`.
	#>
	GIT_SSL_NO_VERIFY=true git clone \
		-b ${git_branch} \
		`echo $1 | sed -e 's|^git+||'` $name
}

git:revision() {
	#<
	# #### `git:revision()`
	# 
	# Get the number of commits in a git repository, on branch 
	# `$git_branch`.
	#>
	cd "$1"
	git checkout ${git_branch}
	git rev-list $(git branch | sed "/^\*/!d;s/^\* //") | wc -l
}

git:pull() {
	#<
	# #### `git:pull()`
	# 
	# Updates a git repository.
	#>
	git checkout ${git_branch}
	if [[ "`git pull`" = "Already up-to-date." ]]; then
		return 1
	fi
}

git:lastver() {
	#<
	# #### `git:lastver()`
	# 
	# Checks whether a local repository is up to date, compared to a
	# remote repository.
	# 
	# Use it in `$lastver`.
	# 
	#     # Example
	#     source=(git://pkgxx.org/pkgxx.git)
	#     lastver="git:lastver"
	#>
	local last_sha current_sha
	if [[ -e "$PKGMK_SOURCE_DIR/$name" ]]; then
		cd $PKGMK_SOURCE_DIR/$name
	else
		# FIXME: Try to compare to any built package.
		echo "$version"
		return 1
	fi
	last_sha="$(git ls-remote "$1" -h refs/heads/$git_branch | sed -e 's|	*refs/heads/'"$git_branch"'$||')"
	current_sha="$(git rev-list $git_branch | head -n 1)"
	if [[ "$last_sha" != "$current_sha" ]]; then
		echo "$version+1"
	else
		echo "$version"
	fi
}

