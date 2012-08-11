
#include "error.sh.h"

/* We include zsh tools to display colors, as any civilised being would */
autoload -U colors
colors

#include "lang.sh.c"
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
#include "splits.sh.c"
#include "signatures.sh.c"

#include "build.sh.c"
#include "install.sh.c"
#include "opts.sh.c"
#include "clean.sh.c"

#include "recursive.sh.c"

interrupted() {
	echo ""
	error "$msg_interrupted"
	
	if [[ "$PKGMK_KEEP_WORK" = "no" ]]; then
		rm -rf $PKGMK_WORK_DIR
	fi
	
	exit E_GENERAL
}

main() {
	local FILE TARGET
	export EXT=""
	
	load_locales
	
	parse_options "$@"
	
	/* 
	 * Profiles are configuration sets used only in some particular cases.
	 * It allows the user to have multiple configuration and to go on
	 * from one another by just doing a pkg++ -p profile.
	 *
	 * As they exist to change the configuration, they must be managed
	 * as soon as possible.
	 */
	if [[ -n "$PKGMK_PROFILE" ]]; then
		for DIR in ${PKGMK_PROFILES_DIRS[@]}; do
			if [[ -e "$DIR/$PKGMK_PROFILE" ]]; then
				. "$PKGMK_PROFILES_DIR/$PKGMK_PROFILE"
			else
				error "$msg_unavailable_profile" "$PKGMK_PROFILE"
				exit 1
			fi
		done
	fi
	
	if [[ "$PKGMK_LIST_INCLUDES" = "yes" ]]; then
		list_includes
		exit 0
	fi
	
	if [[ "$PKGMK_RECURSIVE" = "yes" ]]; then
		recursive "$@"
		exit 0
	fi
	
	for FILE in $(ls "$PKGMK_MODULES_DIR"); do
		. "$PKGMK_MODULES_DIR/$FILE"
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
	
	PKGMK_PKGFILE="`get_pkgfile`"
	
	/*
	 * Configuration file and Pkgfile are sourced.
	 * Note: The configuration file is sourced twice, to allow users to 
	 *       change configuration depending on the package. And probably
	 *       for something else, but I don’t remember what.
	 */
	for FILE in "$PKGMK_CONFFILE" "$PKGMK_PKGFILE"; do
		if [[ ! -f "$FILE" ]]; then
			error "$msg_unavailable" "$FILE"
			exit E_GENERAL
		fi
	done
	
	if [[ -z "${PKGMK_RECIPES[@]}" ]]; then
		error "$msg_no_recipe_format"
		exit E_GENERAL
	fi
	
	source "$PKGMK_CONFFILE"
	for RECIPE in ${PKGMK_RECIPES[@]}; do
		if $RECIPE:match "$PKGMK_PKGFILE"; then
			$RECIPE:parse "$PKGMK_PKGFILE"
			PKGMK_RECIPE_FORMAT=$RECIPE
			break
		fi
	done
	if [[ -z "$PKGMK_RECIPE_FORMAT" ]]; then
		error "$msg_cannot_be_parsed" "$PKGMK_PKGFILE"
		exit E_GENERAL
	fi
	source "$PKGMK_CONFFILE"
	
	/* 
	 * We export $pkgname, to be sure it will be available to modules,
	 * now that the recipe has been parsed.
	 */
	: ${pkgname:=$name}
	
	/* 
	 * Once the recipes are parsed and pkgname is exported, post-recipe
	 * defaults are read.
	 */
	for DIR in ${PKGMK_PRDEFAULTS_DIRS[@]}; do
		if [[ -d $DIR ]] && [[ -n $(ls $DIR/) ]]; then
			for FILE in $DIR"/"*; do
				. $FILE
			done
		fi
	done
	
	/*
	 * Some often used files or directories names.
	 */
	PKGMK_CHANGELOG="`get_metafile "$PKGMK_CHANGELOG"`"
	PKGMK_FOOTPRINT="`get_metafile "$PKGMK_FOOTPRINT"`"
	PKGMK_NOSTRIP="`get_metafile "$PKGMK_NOSTRIP"`"
	PKGMK_POST_INSTALL="`get_metafile "$PKGMK_POST_INSTALL"`"
	PKGMK_PRE_INSTALL="`get_metafile "$PKGMK_PRE_INSTALL"`"
	PKGMK_POST_REMOVE="`get_metafile "$PKGMK_POST_REMOVE"`"
	PKGMK_PRE_REMOVE="`get_metafile "$PKGMK_PRE_REMOVE"`"
	
	PKGMK_PORT="$PKGMK_ROOT/$(dirname "$PKGMK_PKGFILE")"
	
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
		check_pkgfile_full
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
	
	check_pkgfile_core || exit
	
	/* 
	 * If the user wants a dependencies-check, we do it between the recipe check
	 * and the use of it’s content.
	 */
	if ! (
			istrue "$PKGMK_DOWNLOAD_ONLY"         || \
			istrue "$PKGMK_EXTRACT_ONLY"          || \
			istrue "$PKGMK_UP_TO_DATE"            || \
			istrue "$PKGMK_CLEAN"                 || \
			istrue "$PKGMK_UPDATE_FOOTPRINT"      || \
			istrue "$PKGMK_UPDATE_CONTROL_SUMS"   || \
			istrue "$PKGMK_CHECK_PKGFILE"         || \
			istrue "$PKGMK_LIST_INCLUDES"
	   ) && [[ "$PKGMK_CHECK_DEPENDS" = "yes" ]]
	then
		if [[ "$(type ${PKGMK_PACKAGE_MANAGER}:checkdeps)" != "none" ]]; then
			/* It does have access to ${depends[*]}, ${build_depends[*]}, etc. */
			if ! ${PKGMK_PACKAGE_MANAGER}:checkdeps; then
				error "$msg_missing_dependencies"
				exit 1
			fi
		fi
	fi
	
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
	 * At that point, we need to get the revision of the VCS if one is used.
	 */
	for SOURCE in ${source[@]}; do
		local PROTOCOL="$(get_protocol "$SOURCE")"
		if [[ "$(type ${PROTOCOL}:revision)" == "function" ]]; then
			export PKGMK_REVISION="$(${PROTOCOL}:revision "$(get_filename "$SOURCE")")"
		fi
	done
	
	
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
	
	if [[ "$PKGMK_UPDATE_CONTROL_SUMS" = "yes" ]]; then
		download_source
		
		for ALG in ${PKGMK_CONTROL_SUMS[@]}; do
			make_control_sum ${ALG} > "$(get_metafile .${ALG})" && \
				info "$msg_control_sum_updated" "${ALG}"
		done
		
		exit 0
	fi
	
	if [[ "$PKGMK_DOWNLOAD_ONLY" = "yes" ]]; then
		download_source
		exit 0
	fi
	
	if [[ "$PKGMK_EXTRACT_ONLY" = "yes" ]]; then
		download_source
		make_work_dir
		info "$msg_extracting_sources" "$name-$version"
		unpack_source
		exit 0
	fi
	
	/*
	 * Sometimes it is useful to know what has still to be built.
	 */
	if [[ "$PKGMK_UP_TO_DATE" = "yes" ]]; then
		if [[ "`build_needed`" = "yes" ]]; then
			info "$msg_not_up_to_date" "$TARGET"
		else
			info "$msg_up_to_date" "$TARGET"
		fi
		exit 0
	fi
	
	/*
	 * Basic users (n00bs) don’t need to rebuild a package that is
	 * available in the package dir.
	 */
	if [[ "`build_needed`" = "no" && "$PKGMK_FORCE" = "no" && "$PKGMK_CHECK_CONTROL_SUMS" = "no" && "$version" != "devel" ]]; then
		info "$msg_up_to_date" "$TARGET"
	else
		interact_uses "${iuse[*]}"
		download_source
		/* 
		 * Here we try to avoid problems of non-matching versions
		 * with devel packages
		 */
		TARGET="$(get_target)"
		build_packages
		sign_packages
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
 * Don’t want to manage other locales, for now.
 */
export PKGMK_LOCALE="${LC_ALL:-$LANG}"
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
/* Post-recipe defaults. */
PKGMK_PRDEFAULTS_DIRS=(_SHAREDIR/pkg++/post-defaults _SYSCONFDIR/pkg++/post-defaults/)
PKGMK_INCLUDES_DIR=_SHAREDIR"/pkg++/includes"
PKGMK_MODULES_DIR=_SHAREDIR"/pkg++/modules"
PKGMK_PROFILES_DIRS=(_SHAREDIR/pkg++/profiles _SYSCONFDIR/pkg++/profiles)
PKGMK_LOCALES_DIR=_SHAREDIR"/pkg++/locales"
PKGMK_PKGFILE_NAME="Pkgfile"
PKGMK_PKGFILE=""
PKGMK_RECIPE_FORMAT=""
PKGMK_PROFILE=""
PKGMK_CHANGELOG="ChangeLog"
PKGMK_FOOTPRINT=".footprint"
PKGMK_CONTROL_SUMS=(md5sum sha256sum)
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
PKGMK_IGNORE_FOOTPRINT="yes"
PKGMK_IGNORE_NEW="no"
PKGMK_FORCE="no"
PKGMK_KEEP_WORK="no"
PKGMK_UPDATE_CONTROL_SUMS="no"
PKGMK_IGNORE_CONTROL_SUMS="no"
PKGMK_CHECK_CONTROL_SUMS="no"
PKGMK_NO_STRIP="no"
PKGMK_CLEAN="no"
PKGMK_CHECK="no"
PKGMK_CHECK_PKGFILE="no"
PKGMK_DEBUG="no"
PKGMK_NOFAIL="no"
PKGMK_KEEP_ERRLOGS="yes"
PKGMK_CHECK_DEPENDS="yes"

/* 
 * Variables used by generic defaults and post-defaults files.
 */
PKGMK_HOMEPREFIX="no"

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
