monotone:clone() {
	mtn clone $1 $name
}

monotone:revision() {
	cd "$1"
	mtn automate log | wc-l 
}

monotone:pull() {
}

