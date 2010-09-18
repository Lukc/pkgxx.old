
list_includes() {
	/*
	 * A nice list of inclusion files, with color for different types of 
	 * included content.
	 */
	for include in `ls @SHAREDIR@/pkg++/includes/`; do
		if grep -q "source=" @SHAREDIR@/pkg++/includes/$include; then
			echo -e "\033[34m$include\033[00m"
		elif grep -q "build()" @SHAREDIR@/pkg++/includes/$include; then
			echo -e "\033[32m$include\033[00m"
		else
			echo -e "$include"
		fi
	done
}

