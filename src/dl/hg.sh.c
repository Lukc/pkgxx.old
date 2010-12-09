
hg:clone() {
	hg pull `echo $1 | sed -e "s|hg:hg|hg|"` $name
}

hg:pull() {
	if [[ "`hg pull | tail -n 1`" = "no changes found" ]]; then
		return 1
	else
		hg update > /dev/null
	fi
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
