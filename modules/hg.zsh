
hg:clone() {
	hg clone `echo $1 | sed -e 's|^hg+||'` $name
}

hg:pull() {
	if [[ "`hg pull | tail -n 1`" = "no changes found" ]]; then
		return 1
	else
		hg update > /dev/null
	fi
}

