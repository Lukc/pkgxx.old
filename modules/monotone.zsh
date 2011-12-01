
# Monotone module contributed by Pingax.

mtn:clone() {
	mtn clone $1 $name
}

mtn:revision() {
	cd "$1"
	mtn automate log -q | wc -l 
}

mtn:pull() {
	if [[ "`mtn automate pull --dry-run -q | grep revision`" = "receive_revision "0"" ]]; then
		return 1
	fi
}

