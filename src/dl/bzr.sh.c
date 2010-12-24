
bzr:clone() {
	bzr branch `echo $1 | sed -e 's|bzr+||;s|bzr:bzr|bzr|'` $name
}

bzr:pull() {
	if [[ "`bzr update 2>&1`" =~ "Tree is up to date at revision ".*" of branch ".* ]]; then
		return 1
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
