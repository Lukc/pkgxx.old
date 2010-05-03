
WARNING="WARNING"
ERROR="ERROR"

NO_NAME="Variable 'name' not specified in \$PKGMK_PKGFILE."
NO_VERSION="Variable 'version' not specified in \$PKGMK_PKGFILE."
NO_RELEASE="Variable 'version' not specified in \$PKGMK_PKGFILE."
NO_BUILD="Function 'build' not specified in \$PKGMK_PKGFILE."

BAD_ARCH="This package is not made to work on your harware."
BAD_KERNEL="This package is not made to work on your kernel."

NO_CHECK="Function 'check' not specified in \$PKGMK_PKGFILE."

DIR_DOES_NOT_EXIST="Directory '\$1' does not exist."
DIR_NOT_WRITABLE="Directory '\$1' not writable."
DIR_NOT_READABLE="Directory '\$1' not readable."

FILE_NOT_WRITABLE="File '\$1' is not writable."

GETTING="Getting '\$1'."

WGET_NOT_FOUND="Command 'wget' not found."
PARTIAL_DOWNLOAD="Partial download found, trying to resume"
PARTIAL_DOWNLOAD_FAILED="Partial download failed, restarting"
DOWNLOAD_FAILED="Downloading '\$1' failed."

SOURCE_FILE_NOT_FOUND_NO_URL="Source file '\$LOCAL_FILENAME' not found (can not be downloaded, URL not specified)."
SOURCE_FILE_NOT_FOUND="Source file '\$LOCAL_FILENAME' not found (use option -d to download)."

KEEP_ARCHIVES_TRUE="KEEP_ARCHIVES set to true in the Pkgfile: no archive extraction."

BUILDING_FAILED="Building '\$TARGET' failed."

MD5SUM_MISMATCH="Md5sum mismatch found"
MD5SUM_NOT_OK="Md5sum not ok."
MD5SUM_NOT_FOUND="Md5sum not found."
MD5SUM_NOT_FOUND_CREATING_NEW="Md5sum not found, creating new."
MD5SUM_OK="Md5sum ok."

SHA256SUM_NOT_OK="Sha256sum not ok."
SHA256SUM_NOT_FOUND="Sha256sum not found."
SHA256SUM_NOT_FOUND_CREATING_NEW="Sha256sum not found, creating new."
SHA256SUM_OK="Sha256sum ok."

NEW="NEW"
MISSING="MISSING"

FOOTPRINT_MISMATCH="Footprint mismatch found"
FOOTPRINT_NOT_FOUND="Footprint not found, creating new."
PACKAGE_NOT_FOUND="Package '\$TARGET' was not found."

NOT_ROOT="Packages should be built as root."

BUILDING="Building '\$TARGET'."
TESTING="Testing \$TARGET."
TARGET_TESTED="\$TARGET successfully tested."
TEST_FAILED="Tests of \$TARGET failed."

BUILD_RESULT="Build result"
FOOTPRINT_IGNORED="Footprint ignored."
BUILDING_SUCCEEDED="Building '\$TARGET' succeeded."

INSTALLING="Installing '\$TARGET'."
INSTALLING_SUCCEEDED="Installing '\$TARGET' succeeded."
INSTALLING_FAILED="Installing '\$TARGET' failed."

ENTERING_DIR="Entering directory '\$DIR'."
LEAVING="Leaving directory '\$DIR'."

REMOVING_TARGET="Removing \$TARGET"
REMOVING_LOCAL_FILENAME="Removing \$LOCAL_FILENAME"

UNABLE_TO_UPDATE_FOOTPRINT="Unable to update footprint. File '\$TARGET' not found."
FOOTPRINT_UPDATED="Footprint updated."

INTERRUPTED="Interrupted."

USAGE="usage: \`basename \$PKGMK_COMMAND\` [options]"
OPTIONS="options"

OPTION_I="  -i,   --install             build and install package"
OPTION_U="  -u,   --upgrade             build and install package (as upgrade)"
OPTION_R="  -r,   --recursive           search for and build packages recursively"
OPTION_D="  -d,   --download            download missing source file(s)"
OPTION_DO="  -do,  --download-only       do not build, only download missing source file(s)"
OPTION_EO="  -eo,  --extract-only        do not build, only extract source file(s)"
OPTION_UTD="  -utd, --up-to-date          do not build, only check if package is up to date"
OPTION_UF="  -uf,  --update-footprint    update footprint using result from last build"
OPTION_IF="  -if,  --ignore-footprint    build package without checking footprint"
OPTION_UM="  -um,  --update-md5sum       update md5sum"
OPTION_IM="  -im,  --ignore-md5sum       build package without checking md5sum"
OPTION_CM="  -cm,  --check-md5sum        do not build, only check md5sum"
OPTION_US="  -us,  --update-sha256sum    update sha256sum"
OPTION_IS="  -is,  --ignore-sha256sum    build package without checking sha256sum"
OPTION_CS="  -cs,  --check-sha256sum     do not build, only check sha256sum"
OPTION_NS="  -ns,  --no-strip            do not strip executable binaries or libraries"
OPTION_F="  -f,   --force               build package even if it appears to be up to date"
OPTION_C="  -c,   --clean               remove package and downloaded files"
OPTION_KW="  -kw,  --keep-work           keep temporary working directory"
OPTION_CF="  -cf,  --config-file <file>  use alternative configuration file"
OPTION_P="  -p,   --pkgfile <file>      use a pkgfile other than \`\$PKGMK_PKGFILE'"
OPTION_V="  -v,   --version             print version and exit "
OPTION_H="  -h,   --help                print help and exit"

OPTION_REQUIRES_AN_ARGUMENT="\`basename \$PKGMK_COMMAND\`: option \$1 requires an argument"
VERSION="\`basename \$PKGMK_COMMAND\` (pkgutils) \$PKGMK_VERSION"
INVALID_OPTION="\`basename \$PKGMK_COMMAND\`: invalid option \$1"

FILE_NOT_FOUND="File '\$FILE' not found."

MD5SUM_UPDATED="Md5sum updated."
SHA256SUM_UPDATED="Sha256sum updated."

EXTRACTING_SOURCES="Extracting sources of package '\$name-\$version'."

NOT_UP_TO_DATE="Package '\$TARGET' is not up to date."
PACKAGE_UP_TO_DATE="Package '\$TARGET' is up to date."

