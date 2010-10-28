
print_help() {
	echo "usage: `basename $PKGMK_COMMAND` [$PKGMK_PKGFILE_NAME] [options]"
	echo "options:"
	echo "  -i,   --install             build and install package"
	echo "  -in,  --interactive         ask for use(s) when possible"
	echo "  -u,   --upgrade             build and install package (as upgrade)"
	echo "  -r,   --recursive           search for and build packages recursively"
	echo "  -d,   --download            download missing source file(s)"
	echo "  -do,  --download-only       do not build, only download missing source file(s)"
	echo "  -eo,  --extract-only        do not build, only extract source file(s)"
	echo "  -utd, --up-to-date          do not build, only check if package is up to date"
	echo "  -uf,  --update-footprint    update footprint using result from last build"
	echo "  -if,  --ignore-footprint    build package without checking footprint"
	echo "  -in,  --ignore-new          build package, ignore new files in a footprint missmatch"
	echo "  -um,  --update-md5sum       update md5sum"
	echo "  -im,  --ignore-md5sum       build package without checking md5sum"
	echo "  -cm,  --check-md5sum        do not build, only check md5sum"
	echo "  -us,  --update-sha256sum    update sha256sum"
	echo "  -is,  --ignore-sha256sum    build package without checking sha256sum"
	echo "  -cs,  --check-sha256sum     do not build, only check sha256sum"
	echo "  -ns,  --no-strip            do not strip executable binaries or libraries"
	echo "  -f,   --force               build package even if it appears to be up to date"
	echo "  -c,   --clean               remove package and downloaded files"
	echo "  -cp   --check-pkgfile       do not build, only check \`$PKGMK_PKGFILE_NAME'"
	echo "  -kw,  --keep-work           keep temporary working directory"
	echo "  -cf,  --config-file <file>  use alternative configuration file"
	echo "  -li,  --list-includes       list all inclusion files available"
	echo "  -v,   --version             print version and exit "
	echo "  -h,   --help                print help and exit"
}

parse_options() {
	local PKGFILE=""
	while [[ "$1" ]]; do
		case $1 in
			-i|--install)
				PKGMK_INSTALL="install" ;;
			-in|--interactive)
				PKGMK_INTERACTIVE="yes" ;;
			-u|--upgrade)
				PKGMK_INSTALL="upgrade" ;;
			-r|--recursive)
				PKGMK_RECURSIVE="yes" ;;
			-d|--download)
				PKGMK_DOWNLOAD="yes" ;;
			-do|--download-only)
				PKGMK_DOWNLOAD="yes"
				PKGMK_DOWNLOAD_ONLY="yes" ;;
			-eo|--extract-only)
				PKGMK_EXTRACT_ONLY="yes" ;;
			-utd|--up-to-date)
				PKGMK_UP_TO_DATE="yes" ;;
			-uf|--update-footprint)
				PKGMK_UPDATE_FOOTPRINT="yes" ;;
			-if|--ignore-footprint)
				PKGMK_IGNORE_FOOTPRINT="yes" ;;
			-in|--ignore-new)
				PKGMK_IGNORE_NEW="yes" ;;
			-um|--update-md5sum)
				PKGMK_UPDATE_MD5SUM="yes" ;;
			-im|--ignore-md5sum)
				PKGMK_IGNORE_MD5SUM="yes" ;;
			-cm|--check-md5sum)
				PKGMK_CHECK_MD5SUM="yes" ;;
			-us|--update-sha256sum)
				PKGMK_UPDATE_SHA256SUM="yes" ;;
			-is|--ignore-sha256sum)
				PKGMK_IGNORE_SHA256SUM="yes" ;;
			-cs|--check-sha256sum)
				PKGMK_CHECK_SHA256SUM="yes" ;;
			-ns|--no-strip)
				PKGMK_NO_STRIP="yes" ;;
			-f|--force)
				PKGMK_FORCE="yes" ;;
			-c|--clean)
				PKGMK_CLEAN="yes" ;;
			-kw|--keep-work)
				PKGMK_KEEP_WORK="yes" ;;
			-cp|--check-pkgfile)
				PKGMK_CHECK_PKGFILE="yes";;
			-cf|--config-file)
				if [[ ! "$2" ]]; then
					echo "`basename $PKGMK_COMMAND`: option $1 requires an argument"
					exit 1
				fi
				PKGMK_CONFFILE="$2"
				shift ;;
			-li|--list-includes)
				PKGMK_LIST_INCLUDES="yes";;
			-v|--version)
				echo "`basename $PKGMK_COMMAND` $PKGMK_VERSION"
				exit 0 ;;
			-h|--help)
				print_help
				exit 0 ;;
			*)
				if [[ -n "$PKGFILE" ]]; then
					echo "`basename $PKGMK_COMMAND`: only one $PKGMK_PKGFILE_NAME can be given"
					exit 1
				elif [[ -r "$1" ]]; then
					PKGFILE="$1"
				else
					echo "`basename $PKGMK_COMMAND`: invalid option $1"
					exit 1
				fi ;;
		esac
		shift
	done
	PKGMK_PKGFILE="$PKGFILE"
}

