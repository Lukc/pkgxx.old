include() {
	for FILE in $@; do
		for DIR in $PKGMK_ROOT $PKGMK_ROOT/../includes $PKGMK_ROOT/../../includes ${PKGMK_INCLUDES_DIR}; do
			if [[ -e $DIR/$FILE ]]; then
				if . $DIR/$FILE; then
					continue 2
				else
					error "An error occured while including \`$DIR/$FILE'."
					return 1 # Parsing failed.
				fi
			fi
		done
		error "Include \`$FILE' was not found."
		return 2 # Not found.
	done
}

list_includes() {
	# A nice list of inclusion files, with color for different types of 
	# included content.
	for include in `ls _SHAREDIR/pkg++/includes/`; do
		if grep -q "source=" _SHAREDIR/pkg++/includes/$include; then
			echo -e "\033[34m$include\033[00m"
		elif grep -q "build()" _SHAREDIR/pkg++/includes/$include; then
			echo -e "\033[32m$include\033[00m"
		else
			# Libraries and tools
			echo -e "\033[36m$include\033[00m"
		fi
	done
}

# vim:set syntax=sh shiftwidth=4 tabstop=4:
