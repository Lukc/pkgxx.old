
SVN_OPTS=(--non-interactive --trust-server-cert)

svn:clone() {
	svn co ${SVN_OPTS[*]} `echo "$1" | sed -e 's|^svn+||'` $name
}

svn:pull() {
	# No need to set the LANG, as it is already exported in src/main.sh.c.
	if [[ "`svn up $SVN_OPTS`" =~ 'At revision '*'.' ]]; then
		return 1
	fi
}

