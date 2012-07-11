
md5sum:sum() {
	if [[ "$md5sum_gnu" = "true" ]]; then
		md5sum $@ | sed -e 's|  .*/|  |'
	else
		for FILE in $@; do
			local md5=$(md5 $FILE | sed -e 's|  .*|  |;s/MD5 (.*) = //')
			echo "$md5  $(basename $FILE)"
		done
	fi
}

sha256sum:sum() {
	if [[ "$sha256sum_gnu" = "true" ]]; then
		sha256sum $@ | sed -e 's|  .*/|  |'
	else
		for FILE in $@; do
			local sha256=$(sha256 $FILE | sed -e 's|  .*|  |;s/SHA256 (.*) = //')
			echo "$sha256  $(basename $FILE)"
		done
	fi
}

