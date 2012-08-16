
print_help() {
	echo "usage: `basename $PKGMK_COMMAND` [$PKGMK_PKGFILE_NAME] [options]"
	echo "options:"
	echo "  -i,   --install               build and install package"
	echo "  -ia,  --interactive           ask for use(s) when possible"
	echo "  -ir,  --install-root <dir>    install packages in an alternative directory"
	echo "  -u,   --upgrade               build and install package (as upgrade)"
	echo "  -r,   --recursive             search for and build packages recursively"
	echo "  -d,   --download              download missing source file(s)"
	echo "  -do,  --download-only         do not build, only download missing source file(s)"
	echo "  -eo,  --extract-only          do not build, only extract source file(s)"
	echo "  -utd, --up-to-date            do not build, only check if package is up to date"
	echo "  -uf,  --update-footprint      update footprint using result from last build"
	echo "  -if,  --ignore-footprint      build package without checking footprint"
	echo "  -in,  --ignore-new            build package, ignore new files in a footprint missmatch"
	echo "  -us,  --update-sums           update control sums"
	echo "  -is,  --ignore-sums           build package without checking control sums"
	echo "  -cs,  --check-sums            do not build, only check control sums"
	echo "  -ns,  --no-strip              do not strip executable binaries or libraries"
	echo "  -f,   --force                 build package even if it appears to be up to date"
	echo "  -c,   --clean                 remove package and downloaded files"
	echo "  -cp   --check-pkgfile         do not build, only check \`$PKGMK_PKGFILE_NAME'"
	echo "  -kw,  --keep-work             keep temporary working directory"
	echo "  -cd,  --check-depends         check that all dependencies are installed before build"
	echo "  -cf,  --config-file <file>    use alternative configuration file"
	echo "  -li,  --list-includes         list all inclusion files available"
	echo "  -p,   --profile <profile>     load a predefined profile"
/*	echo "  -pm,  --package-manager <pm>  set the package manager to use" */
	echo "  -dg,  --debug                 print debug informations while building packages."
	echo "  -nf,  --no-fail               run pkg++ in the dangerous no-fail mode."
	echo "  -v,   --version               print version and exit "
	echo "  -h,   --help                  print help and exit"
}

parse_options() {
	local PKGFILE=""
	while (( $# > 0 )); do
		case $1 in
			-i|--install)
				PKGMK_INSTALL="install" ;;
			-ia|--interactive)
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
			-ir|--install-root)
				if [[ -z "$2" ]]; then
					echo "`basename $PKGMK_COMMAND`: option $1 requires an argument"
					exit E_GENERAL
				fi
				PKGMK_INSTALL_ROOT="$2"
				shift ;;
			/* m for md5sum, for Crux compatibility */
			-um|-us|--update-sums)
				PKGMK_UPDATE_CONTROL_SUMS="yes" ;;
			-im|-is|--ignore-sums)
				PKGMK_IGNORE_CONTROL_SUMS="yes" ;;
			-cm|-cs|--check-sums)
				PKGMK_CHECK_CONTROL_SUMS="yes" ;;
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
			-cd|--check-depends)
				PKGMK_CHECK_DEPENDS="yes" ;;
			-cf|--config-file)
				if [[ -z "$2" ]]; then
					echo "`basename $PKGMK_COMMAND`: option $1 requires an argument"
					exit E_GENERAL
				fi
				PKGMK_CONFFILE="$2"
				shift ;;
			-li|--list-includes)
				PKGMK_LIST_INCLUDES="yes";;
			-p|--profile)
				if [[ -z "$2" ]]; then
					echo "`basename $PKGMK_COMMAND`: option $1 requires an argument"
					exit E_GENERAL
				fi
				PKGMK_PROFILE="$2"
				shift ;;
/*			-pm|--package-manager)
 *				if [[ -z "$2" ]]; then
 *					echo "`basename $PKGMK_COMMAND`: option $1 requires an argument"
 *					exit E_GENERAL
 *				fi
 *				PKGMK_PACKAGE_MANAGER="$2"
 *				shift ;; */
			-dg|--debug)
				PKGMK_DEBUG="yes" ;;
			-nf|--no-fail)
				PKGMK_NOFAIL="yes" ;;
			-v|--version)
				echo "`basename $PKGMK_COMMAND` $PKGMK_VERSION"
				exit 0 ;;
			-h|--help)
				print_help
				exit 0 ;;
			*)
				if [[ -n "$PKGFILE" ]]; then
					echo "`basename $PKGMK_COMMAND`: only one $PKGMK_PKGFILE_NAME can be given"
					exit E_GENERAL
				elif [[ -r "$1" ]]; then
					PKGFILE="$1"
				else
					echo "`basename $PKGMK_COMMAND`: invalid option $1"
					exit E_GENERAL
				fi ;;
		esac
		shift
	done
	PKGMK_PKGFILE="$PKGFILE"
}

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
