
info() {
	echo -e "\033[32m=======> $1\033[00m"
}

warning() {
	echo -e "\033[33m=======> WARNING: $1\033[00m" >&2
}

error() {
	echo -e "\033[31m=======> ERROR: $1\033[00m" >&2
}

