
get_split_data() {
	/* 
	 * This is used mainly in build().
	 */
	local SPLIT="${1}"
	local VAR="${2}"
	local VAR="$(eval "$(echo "\$${SPLIT}_${VAR}")")"
	if [[ -z "$VAR" ]]; then
		echo "$(eval "$(echo "\$${pkgname}_${VAR}")")"
	else
		echo "$VAR"
	fi
}

list_splits() {
	/* 
	 * This is currently used only in main().
	 */
	for SPLIT in $name ${splits[@]}; do
		eval "
			if [[ -z \"\${${SPLIT}_pkgname}\" ]]; then
				if [[ ${SPLIT} == $name ]]; then
					export ${SPLIT}_pkgname=\"${pkgname}\"
				else
					export ${SPLIT}_pkgname=\"${SPLIT}\"
				fi
			fi
		"
		
		for VAR in name version release pkgname license; do
			eval "
				export split_${VAR}=\"\$${SPLIT}_${VAR}\"
				if [[ -z \"\$split_${VAR}\" ]]; then
					split_${VAR}=\"\${${VAR}}\"
				fi
			"
		done
		
		for VAR in source archs kernels; do
			eval "
				export split_${VAR}
				split_${VAR}=(\${${SPLIT}_${VAR}[@]})
				if [[ \${#split_${VAR}} = 0 ]]; then
					split_${VAR}=(\${${VAR}[@]})
				fi
			"
		done
		pkgname="${split_pkgname}"              \
		name="${split_name}"                    \
		version="${split_version}"              \
		release="${split_release}"              \
		license="${split_license}"              \
		archs=(${split_depends[@]})             \
		kernels=(${split_kernels[@]})           \
		ARCH="$(target_arch)"                   \
		KERNEL="$(target_kernel)"               \
			${PKGMK_PACKAGE_MANAGER}:target
	done
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
