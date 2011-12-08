
git:clone() {
	GIT_SSL_NO_VERIFY=true git clone `echo $1 | sed -e 's|^git+||'` $name
}

git:revision() {
	cd "$1"
	git rev-list $(git branch | sed "/^\*/!d;s/^\* //") | wc -l
}

git:pull() {
	if [[ "`git pull`" = "Already up-to-date." ]]; then
		return 1
	fi
}

# This special function provides a tool to check if a local repository is up
#+ to date and to print a warning if not… when put in a nice $lastver.
git:lastver() {
	local last_sha current_sha
	if [[ -e "$PKGMK_SOURCE_DIR/$name" ]]; then
		cd $PKGMK_SOURCE_DIR/$name
	else
		#error "Can not check whether repository is up to date without a local clone."
		echo "$version"
		return 1
	fi
	last_sha="$(git ls-remote "$1" -h refs/heads/master | sed -e 's|	*refs/heads/master$||')"
	current_sha="$(git rev-list master | head -n 1)"
	if [[ "$last_sha" != "$current_sha" ]]; then
		echo "$version+1"
	else
		echo "$version"
	fi
}

