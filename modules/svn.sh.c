
svn:clone() {
	svn co `echo "$1" | sed -e 's|svn+||;s|svn:svn|svn|'` $name
}

svn:pull() {
	if [[ "`LANG=en_US.utf8 svn up`" =~ 'At revision '*'.' ]]; then
		return 1
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
