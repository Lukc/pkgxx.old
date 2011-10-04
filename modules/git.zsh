
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

