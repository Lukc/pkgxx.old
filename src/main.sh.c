
#include "splits.sh.h"

#include "error.sh.h"
#include "display.sh.c"
#include "getters.sh.c"
#include "tools.sh.c"
#include "tar.sh.c"
#include "checks.sh.c"
#include "includes.sh.c"
#include "downloads.sh.c"
#include "unpack.sh.c"
#include "checksums.sh.c"
#include "footprints.sh.c"
#include "strip.sh.c"
#include "manpages.sh.c"
#include "work.sh.c"
#include "interactions.sh.c"

#include "build.sh.c"
#include "install.sh.c"
#include "opts.sh.c"

recursive() {
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

list_splits() {
	export_splits
	for i in ${!PKG_NAMES[*]}; do
		split_exec \
			$PKGMK_PACKAGE_MANAGER:target
	done
}

clean() {
	/*
	 * clean() removes the sources in $PKGMK_SOURCE_DIR and the package, if
	 * it is in $PKGMK_PACKAGE_DIR.
	 */
	local FILE LOCAL_FILENAME
	
	for LOCAL_FILENAME in $(list_splits); do
		if [[ -f "$LOCAL_FILENAME" ]]; then
			info "Removing $LOCAL_FILENAME"
			rm -f "$LOCAL_FILENAME"
		fi
	done
	
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
	echo ""
	error "Interrupted."
	
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		rm -rf $PKGMK_WORK_DIR
	fi
	
	exit E_GENERAL
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
	
	/*
	 * Some often used files or directories names.
	 */
	PKGMK_PKGFILE="`get_pkgfile`"
	
	/* In case the version is in the filenae */
	version="$(echo $PKGMK_PKGFILE | sed 's/.*-//')"
	
	PKGMK_CHANGELOG="`get_metafile "$PKGMK_CHANGELOG"`"
	PKGMK_FOOTPRINT="`get_metafile "$PKGMK_FOOTPRINT"`"
	PKGMK_MD5SUM="`get_metafile "$PKGMK_MD5SUM"`"
	PKGMK_SHA256SUM="`get_metafile "$PKGMK_SHA256SUM"`"
	PKGMK_NOSTRIP="`get_metafile "$PKGMK_NOSTRIP"`"
	PKGMK_POST_INSTALL="`get_metafile "$PKGMK_POST_INSTALL"`"
	PKGMK_PRE_INSTALL="`get_metafile "$PKGMK_PRE_INSTALL"`"
	PKGMK_POST_REMOVE="`get_metafile "$PKGMK_POST_REMOVE"`"
	PKGMK_PRE_REMOVE="`get_metafile "$PKGMK_PRE_REMOVE"`"
	
	PKGMK_PORT="$PKGMK_ROOT/$(dirname "$PKGMK_PKGFILE")"
	
	for FILE in $(ls "$PKGMK_MODULES_DIR"); do
		. $PKGMK_MODULES_DIR/$FILE
	done
	
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
	for FILE in "$PKGMK_CONFFILE" "$PKGMK_PKGFILE" "$PKGMK_CONFFILE"; do
		if [[ ! -f "$FILE" ]]; then
			error "File '$FILE' not found."
			exit E_GENERAL
		fi
		if [[ "$(dirname "$FILE")" = "." ]]; then
			. "./$FILE"
		else
			. "$FILE"
		fi
	done
	
	/*
	 * We need to define a group, with some package managers. If we don’t 
	 * give them a group, they will attack us, kill kittens and won’t 
	 * make the package.
	 */
	if [[ -z "${groups[@]}" ]] && has $PKGMK_PACKAGE_MANAGER ${PKGMK_PM_NEEDS_GROUP[@]}; then
		export groups
		groups=($(basename `dirname $PWD/${PKGMK_PKGFILE%$PKGMK_PKGFILE_NAME}`))
	fi
	
	/*
	 * Bah, just in case the configuration is not usable.
	 */
	check_config
	
	for INCLUDE in ${includes[@]}; do
		include $INCLUDE
	done
	
	/*
	 * If -cp or --check-pkgfile, we just check the quality of the recipe.
	 */
	if [[ "$PKGMK_CHECK_PKGFILE" = "yes" ]]; then
		check_pkgfile_only
	fi
	
	/*
	 * It would be sad to try working on directories we don’t own.
	 */
	check_directory "$PKGMK_SOURCE_DIR"
	check_directory "$PKGMK_PACKAGE_DIR"
	check_directory "`dirname $PKGMK_WORK_DIR`"
	
	/*
	 * It is very important to check that the tools we will use are here, 
	 * because we don’t want to be alone and miserably fail. 
	 */
	if [[ "$(type "${PKGMK_PACKAGE_MANAGER}:checks")" != none ]]; then
		${PKGMK_PACKAGE_MANAGER}:checks
	fi
	
	check_pkgfile
	
	/* 
	 * We set the architecture to something known by the package manager.
	 */
	PKGMK_ARCH=$(pm_arch)
	PKGMK_KERNEL=$(pm_kernel)
	
	/* 
	 * And then we set another architecture variable to something known
	 * by the cookers.
	 */
	arch=$(target_arch)
	kernel=$(target_kernel)
	
	/* 
	 * We get the target from a specific function.
	 */
	TARGET="$(get_target)"
	
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
	
	/*
	 * Sometimes it is useful to know what has still to be built.
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
		interact_uses "${iuse[*]}"
		download_source
		/* 
		 * Here we try to avoid problems of non-matching versions
		 * with devel packages
		 */
		TARGET="$(get_target)"
		build_package
	fi
	
	/*
	 * To install or not to install, that is the (last) question.
	 */
	if [[ "$PKGMK_INSTALL" != "no" ]]; then
		install_package
	fi
	
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
 * We export a new path to directly use some tools into pkg++, like has().
 */
export PATH=$PATH:_LIBEXECDIR/pkg++

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
PKGMK_DEFAULTS_DIRS=(_SHAREDIR/pkg++/defaults _SYSCONFDIR/pkg++/defaults/)
PKGMK_INCLUDES_DIR=_SHAREDIR"/pkg++/includes"
PKGMK_MODULES_DIR=_SHAREDIR"/pkg++/modules"
PKGMK_PKGFILE_NAME="Pkgfile"
PKGMK_PKGFILE=""
PKGMK_CHANGELOG="ChangeLog"
PKGMK_FOOTPRINT=".footprint"
PKGMK_MD5SUM=".md5sum"
PKGMK_SHA256SUM=".sha256sum"
PKGMK_NOSTRIP=".nostrip"
PKGMK_POST_INSTALL="post-install"
PKGMK_PRE_INSTALL="pre-install"
PKGMK_POST_REMOVE="post-remove"
PKGMK_PRE_REMOVE="pre-remove"

PKGMK_SOURCE_MIRRORS=()
PKGMK_SOURCE_DIR="$PWD"
PKGMK_PACKAGE_DIR="$PWD"
PKGMK_LOGS_DIR="$PWD"
PKGMK_WORK_DIR="$PWD/work"
PKGMK_INSTALL_ROOT=

PKGMK_COMPRESSION_MODE="xz"
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
PKGMK_DEBUG="no"
PKGMK_NOFAIL="no"
PKGMK_KEEP_ERRLOGS="yes"

PKGMK_KERNEL=_KERNEL

PKGMK_UNTAR_TOOL=gtar
PKGMK_UNRPM_TOOL=rpm
PKGMK_UNRPM_TOOLS=(
	rpm bsdtar
)
PKGMK_UNZIP_TOOL=unzip
PKGMK_UNZIP_TOOLS=(
	unzip bsdtar
)

PKGMK_DOWNLOAD_TOOL=wget
PKGMK_DOWNLOAD_TOOLS=(
	curl axel wget
)

PKGMK_PACKAGE_MANAGER=_PACKAGE_MANAGER

main "$@"

/* vim:set syntax=sh shiftwidth=4 tabstop=4: */
