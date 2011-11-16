monotone:clone() {
	mtn clone $1 $name
}

monotone:revision() {
	cd "$1"
	mtn automate log | wc-l 
}

monotone:pull() {
	if [[ "`mtn automate pull --dry-run -q | grep revision`" = "receive_revision "0"" ]]; then
		return 1
	fi
}
