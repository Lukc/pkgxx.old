
// vim: syntax=sh

#include "error.sh.h"
#include "display.sh.c"
#include "getters.sh.c"
#include "tools.sh.c"
#include "checks.sh.c"
#include "includes.sh.c"
#include "downloads.sh.c"
#include "unpack.sh.c"
#include "checksums.sh.c"
#include "footprints.sh.c"
#include "strip.sh.c"
#include "manpages.sh.c"
#include "work.sh.c"

#if defined pacman
#	include "pacman.sh.c"
#endif

#if defined rpm
#	include "rpm.sh.c"
#endif

#if defined pkgtools
#	include "pkgtools.sh.c"
#endif

#if defined nhopkg
#	include "nhopkg.sh.c"
#endif

#include "build.sh.c"
#include "install.sh.c"
#include "opts.sh.c"

recursive() {
	/*
	 * This function is used with the -r option to rebuild/install/update
	 * all packages in the directory and all it’s subdirectories.
	 */
	local ARGS FILE DIR
	
	/*
	 * Hey, we don’t want to make a recursive fail.
	 */
	ARGS=`echo "$@" | sed -e "s/--recursive//g" -e "s/-r//g"`
	
	/*
	 * Now, for each directory that contains a Pkgfile, we move to it and
	 * we launch pkg++ again, with it’s args.
	 */
	for FILE in `find $PKGMK_ROOT -name $PKGMK_PKGFILE_NAME | sort`; do
		DIR="`dirname $FILE`/"
		if [[ -d $DIR ]]; then
			info "Entering directory '$DIR'."
			(cd $DIR && $PKGMK_COMMAND $ARGS)
			info "Leaving directory '$DIR'."
		fi
	done
}

clean() {
	/*
	 * clean() removes the sources in $PKGMK_SOURCE_DIR and the package, if
	 * it is in $PKGMK_PACKAGE_DIR.
	 */
	local FILE LOCAL_FILENAME
	
	/*
	 * First we remove the package.
	 */
	if [[ -f $TARGET ]]; then
		info "Removing $TARGET"
		rm -f $TARGET
	fi
	
	/*
	 * And then we remove all the source files.
	 */
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		if [[ -e $LOCAL_FILENAME ]] && [[ "$LOCAL_FILENAME" != "$FILE" ]]; then
			info "Removing $LOCAL_FILENAME"
			if [[ -d $LOCAL_FILENAME ]]; then
				rm -r -f $LOCAL_FILENAME
			else
				rm -f $LOCAL_FILENAME
			fi
		fi
	done
}

interrupted() {
	/*
	 * This function is called in case of interruption.
	 * It cleans the work dir and ends pkg++.
	 */
	echo ""
	error "Interrupted."
	
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		rm -rf $PKGMK_WORK_DIR
	fi
	
	exit 1
}

main() {
	/*
	 * Oh yeah, pkg++ begins here. \o/
	 */
	local FILE TARGET ARCH
	export EXT=""
	
	/*
	 * We parse the options.
	 */
	parse_options "$@"
	
	/*
	 * If --list-includes or -li,  we just list the includes.
	 */
	if [[ "$PKGMK_LIST_INCLUDES" = "yes" ]]; then
		list_includes
		exit 0
	fi
	
	/*
	 * Use pkg++ recursively with --recursive or -r.
	 */
	if [[ "$PKGMK_RECURSIVE" = "yes" ]]; then
		recursive "$@"
		exit 0
	fi
	
	/*
	 * Some very used file names.
	 */
	PKGMK_PKGFILE="`get_pkgfile`"
	
	PKGMK_FOOTPRINT="`get_metafile footprint`"
	PKGMK_MD5SUM="`get_metafile md5sum`"
	PKGMK_SHA256SUM="`get_metafile sha256sum`"
	PKGMK_NOSTRIP="`get_metafile nostrip`"
	
	/*
	 * We need to define a group, with pacman and rpm. If we don’t give 
	 * them a group, they will attack us, kill baby cats and won’t make the
	 * package.
	 * Note: We don’t need to check if another group has been already 
	 *       declared, because the Pkgfile is sourced later.
	 */
	#if defined pacman \
	|| defined rpm \
	|| defined nhopkg
	export groups=($(basename `dirname $PWD/${PKGMK_PKGFILE%$PKGMK_PKGFILE_NAME}`))
	#endif
	
	/*
	 * The defaults files. Each file in each dir is sourced.
	 */
	for DIR in ${PKGMK_DEFAULTS_DIRS[@]}; do
		/*
		 * They could potencially have not been created.
		 */
		if [[ -d $DIR ]] && [[ -n $(ls $DIR/) ]]; then
		/*
		 * The quotes are here to avoid the C preprocessor to treat the
		 * "/*" like a begin of comment.
		 */
			for FILE in $DIR"/"*; do
				. $FILE
			done
		fi
	done
	
	/*
	 * Configuration file and Pkgfile are sourced.
	 * Note: The configuration file is sourced twice, to allow users to 
	 *       change configuration depending on the package. And probably
	 *       for something else, but I don’t remember what.
	 */
	for FILE in $PKGMK_CONFFILE $PKGMK_PKGFILE $PKGMK_CONFFILE; do
		if [[ ! -f $FILE ]]; then
			error "File '$FILE' not found."
			exit 1
		fi
		. $FILE
	done
	
	/*
	 * Bah, just in case the configuration is not usable.
	 */
	check_config
	
	/*
	 * We source any file that has been included with ${includes[ ]}.
	 */
	for INCLUDE in ${includes[@]}; do
		. $PKGMK_INCLUDES_DIR/$INCLUDE
	done
	
	/*
	 * If -cp or --check-pkgfile, we just check the quality of the recipe.
	 */
	if [[ "$PKGMK_CHECK_PKGFILE" = "yes" ]]; then
		check_new_pkgfile
	fi
	
	/*
	 * It is very important to check that the tools we will use are here, 
	 * because we don’t want to be alone and miserably fail. 
	 */
	#if defined rpm
	check_command rpm
	check_command rpmbuild
	#elif defined dpkg
	check_command dpkg
	#elif defined pkgtools
	check_command makepkg
	#endif
	/*
	 * We don’t need to check for pacman{,-g2}, because it is not needed to
	 * build the package, like rpmbuild and dpkg are. However, the user
	 * will get an error if using the option -i or -u and pacman{,-g2} is 
	 * not installed.
	 */
	
	/*
	 * It would be sad to try working on directories we don’t own.
	 */
	check_directory "$PKGMK_SOURCE_DIR"
	check_directory "$PKGMK_PACKAGE_DIR"
	check_directory "`dirname $PKGMK_WORK_DIR`"
	#if defined rpm
	/*
	 * RPM is worse, it needs it’s own tree inside the package dir.
	 */
	check_directory "$PKGMK_PACKAGE_DIR/RPM/RPMS/$arch"
	#endif
	
	check_pkgfile
	
	/*
	 * No-arch packages must be of the form -noarch, instead of being of 
	 * the form -x86 or whatever.
	 * Note: pacman-g2 doesn’t support — for now — noarch packages, but 
	 *       pacman does.
	 * Note: If pacmang2 is defined, pacman is also defined. So we need to
	 *       check for the two.
	 */
	#if ! defined pacmang2 && \
	    defined pacman || \
	    defined dpkg   || \
	    defined rpm    || \
	    defined pkgtools || \
	    defined nhopkg
	if has no-arch ${archs[@]} || has no-kernel ${kernels[@]}; then
		ARCH=noarch
	else
		ARCH="$PKGMK_ARCH"
	fi
	#else
	ARCH="$PKGMK_ARCH"
	#endif
	/*
	 * Targets definitions, depending of package manager used.
	 * Note: devel and standard versions always have different targets.
	 */
	#if defined dpkg
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/${name}_devel-`date +%Y%m%d`-${release}_$ARCH.deb"
	else
		TARGET="$PKGMK_PACKAGE_DIR/${name}_$version-${release}_$ARCH.deb"
	fi
	#elif defined rpm
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$ARCH.rpm"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$ARCH.rpm"
	fi
	#elif defined pacman
	/*
	 * The extension change between pacman and pacman-g2.
	 */
	#if defined pacmang2
	EXT="fpm"
	#else
	EXT="pkg.tar.xz"
	#endif
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$ARCH.$EXT"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$ARCH.$EXT"
	fi
	#elif defined pkgtools
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$ARCH-$release.txz"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$ARCH-$release.txz"
	fi
	#elif defined nhopkg
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release.nho"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release.nho"
	fi
	#else
	/*
	 * Pkgutils users will be able to choose their compression method.
	 */
	case $PKGMK_COMPRESSION_MODE in
	gz|bz2|xz|lzo)
		EXT="$PKGMK_COMPRESSION_MODE"
		;;
	none);;
	*)
		error "Compression mode '$PKGMK_COMPRESSION_MODE' not supported"
		exit 1
		;;
	esac
	
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name#devel-`date +%Y%m%d`-$release.pkg.tar"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name#$version-$release.pkg.tar"
	fi
	
	if [[ -n "$EXT" ]]; then
		TARGET="$TARGET.$EXT"
	fi
	#endif
	
	/*
	 * If we just want to remove the already made package and the sources…
	 */
	if [[ "$PKGMK_CLEAN" = "yes" ]]; then
		clean
		exit 0
	fi
	
	/*
	 * … or if we just want to update the footprint…
	 */
	if [[ "$PKGMK_UPDATE_FOOTPRINT" = "yes" ]]; then
		update_footprint
		exit 0
	fi
	
	/*
	 * … or the md5sum…
	 */
	if [[ "$PKGMK_UPDATE_MD5SUM" = "yes" ]]; then
		download_source
		check_file "$PKGMK_MD5SUM"
		make_md5sum > $PKGMK_MD5SUM
		info "Md5sum updated."
	fi
	
	/*
	 * … or the sha256sum.
	 */
	if [[ "$PKGMK_UPDATE_SHA256SUM" = "yes" ]]; then
		download_source
		check_file "$PKGMK_SHA256SUM"
		make_sha256sum > $PKGMK_SHA256SUM
		info "Sha256sum updated."
	fi
	/*
	 * And if we checked any control sum, then we exit.
	 */
	if [[ "$PKGMK_UPDATE_SHA256SUM" = "yes" ]] || [[ "$PKGMK_UPDATE_MD5SUM" = "yes" ]]; then
		exit 0
	fi
	
	/*
	 * For those who need to only get sources. Maybe they will build later.
	 */
	if [[ "$PKGMK_DOWNLOAD_ONLY" = "yes" ]]; then
		download_source
		exit 0
	fi
	
	/*
	 * Uh… for those who need to only extract the sources ?
	 */
	if [[ "$PKGMK_EXTRACT_ONLY" = "yes" ]]; then
		download_source
		make_work_dir
		info "Extracting sources of package '$name-$version'."
		unpack_source
		exit 0
	fi
	
	/*
	 * Sometimes it is usefull to know what has still to be built.
	 */
	if [[ "$PKGMK_UP_TO_DATE" = "yes" ]]; then
		if [[ "`build_needed`" = "yes" ]]; then
			info "Package '$TARGET' is not up to date."
		else
			info "Package '$TARGET' is up to date."
		fi
		exit 0
	fi
	
	/*
	 * Basic users (n00bs) don’t need to rebuild a package that is
	 * available in the package dir.
	 */
	if [[ "`build_needed`" = "no" ]] && [[ "$PKGMK_FORCE" = "no" ]] && [[ "$PKGMK_CHECK_MD5SUM" = "no" ]] && [[ "$version" != "devel" ]]; then
		info "Package '$TARGET' is up to date."
	else
		download_source
		build_package
	fi
	
	/*
	 * To install or not to install, that is the (last) question.
	 */
	if [[ "$PKGMK_INSTALL" != "no" ]]; then
		install_package
	fi
	
	/*
	 * Must I really comment that ?
	 */
	exit 0
}

/*
 * Interrupting pkg++ while building would be a very bad thing. trap redirects
 * interruptions to interrupted().
 */
trap "interrupted" SIGHUP SIGINT SIGQUIT SIGTERM

/*
 * Don’t want to manage other locales.
 */
export LC_ALL=POSIX

/*
 * Some RO functions that will never be modified during the execution.
 */
readonly PKGMK_VERSION=_VERSION
readonly PKGMK_COMMAND="$0"
readonly PKGMK_ROOT="$PWD"

/*
 * Default dirs and files names.
 */
PKGMK_CONFFILE=_SYSCONFDIR"/pkg++.conf"
PKGMK_DEFAULTS_DIRS=(_SHAREDIR/pkg++/defaults _SYSCONFDIR/pkg++)
PKGMK_INCLUDES_DIR=_SHAREDIR"/pkg++/includes"
PKGMK_PKGFILE_NAME="Pkgfile"
PKGMK_PKGFILE=""
PKGMK_FOOTPRINT=".footprint"
PKGMK_MD5SUM=".md5sum"
PKGMK_SHA256SUM=".sha256sum"
PKGMK_NOSTRIP=".nostrip"

PKGMK_SOURCE_MIRRORS=()
PKGMK_SOURCE_DIR="$PWD"
PKGMK_PACKAGE_DIR="$PWD"
PKGMK_WORK_DIR="$PWD/work"

#if defined pacman
PKGMK_COMPRESSION_MODE="xz"
#else
/*
 * The other sucks enough to have a bad default value. :D
 */
PKGMK_COMPRESSION_MODE="gz"
#endif
PKGMK_MAN_COMPRESSION="gz"

/*
 * Pkg++ can receive many instructions.
 */
PKGMK_INSTALL="no"
PKGMK_INTERACTIVE="no"
PKGMK_RECURSIVE="no"
PKGMK_DOWNLOAD="no"
PKGMK_DOWNLOAD_ONLY="no"
PKGMK_EXTRACT_ONLY="no"
PKGMK_UP_TO_DATE="no"
PKGMK_UPDATE_FOOTPRINT="no"
PKGMK_IGNORE_FOOTPRINT="no"
PKGMK_IGNORE_NEW="no"
PKGMK_FORCE="no"
PKGMK_KEEP_WORK="no"
PKGMK_UPDATE_MD5SUM="no"
PKGMK_IGNORE_MD5SUM="no"
PKGMK_CHECK_MD5SUM="no"
PKGMK_UPDATE_SHA256SUM="no"
PKGMK_IGNORE_SHA256SUM="no"
PKGMK_CHECK_SHA256SUM="no"
PKGMK_NO_STRIP="no"
PKGMK_CLEAN="no"
PKGMK_CHECK="no"
PKGMK_CHECK_PKGFILE="no"

PKGMK_ARCH=_ARCH
PKGMK_KERNEL=_KERNEL

readonly PKGMK_DOWNLOAD_TOOLS=(curl wget)
#if defined curl
DOWNLOAD_TOOL="curl"
#else /* wget is the default download tool used. */
DOWNLOAD_TOOL="wget"
#endif

main "$@"

