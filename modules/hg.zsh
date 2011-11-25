
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

