
git:clone() {
	git clone `echo $1 | sed -e 's|git+||;s|git:git|git|'` $name
}

git:pull() {
	if [[ "`git pull`" = "Already up-to-date." ]]; then
		return 1
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
