
// vim: syntax=sh

#include "config.sh.h"

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

#include "build.sh.c"
#include "install.sh.c"
#include "opts.sh.c"

recursive() {
	local ARGS FILE DIR
	
	ARGS=`echo "$@" | sed -e "s/--recursive//g" -e "s/-r//g"`
	
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
	local FILE LOCAL_FILENAME
	
	if [[ -f $TARGET ]]; then
		info "Removing $TARGET"
		rm -f $TARGET
	fi
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		if [[ -e $LOCAL_FILENAME ]] && [[ "$LOCAL_FILENAME" != "$FILE" ]]; then
			info "Removing $LOCAL_FILENAME"
			rm -f $LOCAL_FILENAME
		fi
	done
}

interrupted() {
	echo ""
	error "Interrupted."
	
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		rm -rf $PKGMK_WORK_DIR
	fi
	
	exit 1
}

main() {
	local FILE TARGET
	export EXT=""
	
	parse_options "$@"
	
	if [[ "$PKGMK_LIST_INCLUDES" = "yes" ]]; then
		list_includes
		exit 0
	fi
		
	if [[ "$PKGMK_RECURSIVE" = "yes" ]]; then
		recursive "$@"
		exit 0
	fi
	
	PKGMK_PKGFILE="`get_pkgfile`"
	
	PKGMK_FOOTPRINT="`get_metafile footprint`"
	PKGMK_MD5SUM="`get_metafile md5sum`"
	PKGMK_SHA256SUM="`get_metafile sha256sum`"
	PKGMK_NOSTRIP="`get_metafile nostrip`"
	
	#if defined pacman || defined rpm
	export groups=($(basename `dirname $PWD/${PKGMK_PKGFILE%$PKGMK_PKGFILE_NAME}`))
	#endif
	
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
	
	for FILE in $PKGMK_CONFFILE $PKGMK_PKGFILE $PKGMK_CONFFILE; do
		if [[ ! -f $FILE ]]; then
			error "File '$FILE' not found."
			exit 1
		fi
		. $FILE
	done
	
	#if defined dpkg
	if [[ "$PKGMK_ARCH" = x86 ]]; then
		PKGMK_ARCH="i386"
	elif [[ "$PKGMK_ARCH" = x86_64 ]]; then
		PKGMK_ARCH="amd64"
	fi
	#endif
	
	check_config
	
	for INCLUDE in ${includes[@]}; do
		. $PKGMK_INCLUDES_DIR/$INCLUDE
	done
	
	if [[ "$PKGMK_CHECK_PKGFILE" = "yes" ]]; then
		RETURN=0
		if [[ ! "$description" ]]; then
			error "Variable 'description' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$url" ]]; then
			error "Variable 'url' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$packager" ]]; then
			error "Variable 'packager' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$maintainer" ]]; then
			error "Variable 'maintainer' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$depends" ]]; then
			error "Variable 'depends' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$name" ]]; then
			error "Variable 'name' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$version" ]]; then
			error "Variable 'version' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ ! "$release" ]]; then
			error "Variable 'release' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ "`type -t pre_build`" != "function" ]]; then
			info "Function 'pre_build' not specified in $PKGMK_PKGFILE."
		fi
		if [[ "`type -t build`" != "function" ]]; then
			error "Function 'build' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		if [[ "`type -t post_build`" != "function" ]]; then
			info "Function 'post_build' not specified in $PKGMK_PKGFILE."
		fi
		if [[ "`type -t check`" != "function" ]]; then
			warning "Function 'check' not specified in $PKGMK_PKGFILE."
			RETURN=1
		fi
		exit $RETURN
	fi
	
	#if defined rpm
	check_command rpm
	check_command rpmbuild
	#elif defined dpkg
	check_command dpkg
	#elif defined pkgtools
	check_command makepkg
	#endif
	/*
	 * We don’t need to check for pacman-g2, because it is not needed to 
	 * build the package, like rpmbuild and dpkg are. However, the user
	 * will get an error if using the option -i or -u and pacman-g2 is not
	 * installed.
	 */
	
	check_directory "$PKGMK_SOURCE_DIR"
	check_directory "$PKGMK_PACKAGE_DIR"
	check_directory "`dirname $PKGMK_WORK_DIR`"
	#if defined rpm
	check_directory "$PKGMK_PACKAGE_DIR/RPM/RPMS/$arch"
	#endif
	
	check_pkgfile
	
	#if defined dpkg
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name#devel-`date +%Y%m%d`-$release.deb"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name#$version-$release.deb"
	fi
	#elif defined rpm
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.rpm"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.rpm"
	fi
	#elif defined pacman
	#if defined pacmang2
	EXT="fpm"
	#else
	EXT="pkg.tar.xz"
	#endif
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$release-$PKGMK_ARCH.$EXT"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$release-$PKGMK_ARCH.$EXT"
	fi
	#elif defined pkgtools
	if has no-arch ${archs[@]}; then
		PKGMK_ARCH=noarch
	fi
	if [[ "$version" = "devel" ]] || [[ "$version" = "dev" ]]; then
		TARGET="$PKGMK_PACKAGE_DIR/$name-devel-`date +%Y%m%d`-$PKGMK_ARCH-$release.txz"
	else
		TARGET="$PKGMK_PACKAGE_DIR/$name-$version-$PKGMK_ARCH-$release.txz"
	fi
	#else
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
	
	if [[ "$PKGMK_CLEAN" = "yes" ]]; then
		clean
		exit 0
	fi
	
	if [[ "$PKGMK_UPDATE_FOOTPRINT" = "yes" ]]; then
		update_footprint
		exit 0
	fi
	
	if [[ "$PKGMK_UPDATE_MD5SUM" = "yes" ]]; then
		download_source
		check_file "$PKGMK_MD5SUM"
		make_md5sum > $PKGMK_MD5SUM
		info "Md5sum updated."
	fi
	
	if [[ "$PKGMK_UPDATE_SHA256SUM" = "yes" ]]; then
		download_source
		check_file "$PKGMK_SHA256SUM"
		make_sha256sum > $PKGMK_SHA256SUM
		info "Sha256sum updated."
	fi
	if [[ "$PKGMK_UPDATE_SHA256SUM" = "yes" ]] || [[ "$PKGMK_UPDATE_MD5SUM" = "yes" ]]; then
		exit 0
	fi
	
	if [[ "$PKGMK_DOWNLOAD_ONLY" = "yes" ]]; then
		download_source
		exit 0
	fi
	
	if [[ "$PKGMK_EXTRACT_ONLY" = "yes" ]]; then
		download_source
		make_work_dir
		info "Extracting sources of package '$name-$version'."
		unpack_source
		exit 0
	fi
	
	if [[ "$PKGMK_UP_TO_DATE" = "yes" ]]; then
		if [[ "`build_needed`" = "yes" ]]; then
			info "Package '$TARGET' is not up to date."
		else
			info "Package '$TARGET' is up to date."
		fi
		exit 0
	fi
	
	if [[ "`build_needed`" = "no" ]] && [[ "$PKGMK_FORCE" = "no" ]] && [[ "$PKGMK_CHECK_MD5SUM" = "no" ]] && [[ "$version" != "devel" ]]; then
		info "Package '$TARGET' is up to date."
	else
		download_source
		build_package
	fi
	
	if [[ "$PKGMK_INSTALL" != "no" ]]; then
		install_package
	fi
	
	exit 0
}

trap "interrupted" SIGHUP SIGINT SIGQUIT SIGTERM

export LC_ALL=POSIX

readonly PKGMK_VERSION="@VERSION@"
readonly PKGMK_COMMAND="$0"
readonly PKGMK_ROOT="$PWD"

PKGMK_CONFFILE="@SYSCONFDIR@/pkg++.conf"
PKGMK_DEFAULTS_DIRS=(@SHAREDIR@/pkg++/defaults @SYSCONFDIR@/pkg++)
PKGMK_INCLUDES_DIR="@SHAREDIR@/pkg++/includes"
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
PKGMK_COMPRESSION_MODE="gz"
#endif

PKGMK_INSTALL="no"
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

PKGMK_ARCH="@ARCH@"
PKGMK_KERNEL="@KERNEL@"

readonly PKGMK_DOWNLOAD_TOOLS=(curl wget)
#if defined curl
DOWNLOAD_TOOL="curl"
#else // wget is the default download tool used.
DOWNLOAD_TOOL="wget"
#endif

main "$@"

