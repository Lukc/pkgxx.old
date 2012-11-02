
# Common
msg_func_may_have_failed="%1() may have failed."
msg_file_is_not_directory="\`%1' already exists and is not a directory."
msg_file_not_readable="\`%1' is not readable."
msg_could_not_cd="Couldn't move to \`%1'."

# main.sh.c
msg_interrupted="Interrupted."

msg_control_sum_updated="%1 updated."

msg_extracting_sources="Extracting sources of package \`%1'."

msg_not_up_to_date="Package \`%1' is not up to date."
msg_up_to_date="Package \`%1' is up to date."

msg_unavailable_profile="Requested profile (%1) is not available."
msg_file_not_found="File \`%1' not found."
msg_no_recipe_format="No way to parse any recipe has been found. Please install a recipe module."
msg_cannot_be_parsed="\`%1' can not be parsed."
msg_missing_dependencies="All dependencies where not found on your system."

# build.sh.c
msg_should_build_as_root="Packages should be built as root."

msg_building_package="Building \`%1'%2."
msg_build_success="Building \`%1' succeeded."
msg_build_fail="Building \`%1' failed."

msg_testing="Testing \`%1'."
msg_tests_success="\`%1' was successfully tested."
msg_tests_fail="Tests of \`%1' failed."

msg_log_avail_at="Log available at: %1"

msg_bad_autosplit="Automatic split \`%1' is not available."

msg_build_result="Build result:"

# signatures.sh.c
msg_signing="Signing \`%1'."

# downloads.sh.c
msg_getting="Getting \`%1'."
msg_cannot_fetch_through_protocol="pkg++ cannot fetch sources via \`%1'."
msg_already_u2d="\`%1' already up to date."
msg_localfile_not_found="Source file \`%1' not found (can not be downloaded, URL not specified)."
msg_localfile_not_downloaded="Source file \`%1' not found (use option -d to download)."

# clean.sh
msg_removing_local_filename="Removing \`%1'."

# checksums.sh
msg_algo_notfound="\`%1' not found."

msg_create_algo="\`%1' not found, creating new."

msg_algo_mismatch="\`%1' mistmatch found:"

msg_algo_notok="\`%1' not ok."

msg_build_failed="Building \`%1' failed."

msg_algo_ok="\`%1' ok."


# footprints.sh
msg_new_files_found="New files found:"

msg_footprint_mismatch="Footprint mismatch found:"

msg_create_footprint="Footprint not found, creating new."

msg_package_not_found="Package \`%1' was not found."

msg_unable_update_footprint="Unable to update footprint. File \`%1' not found."

msg_footprint_updated="Footprint updated."


# includes.sh
msg_error_while_including="An error occured while including \`%1'."

msg_include_file_notfound="Include \`%1' was not found."


# install.sh
msg_installing_info="Installing \`%1'."

msg_success_install="Installing \`%1' succeded."

msg_failure_install="Installing \`%1' failed."

# interactions.sh
# TODO: add string for the question
msg_no_useflag_info="There is no available information about use flag \`%1'."


# opts.sh
# TODO: translate print_help() content
# TODO: translate whole file


# recursive.sh
msg_entering_dir="Entering directory \`%1'."

msg_leaving_dir="Leaving directory \`%1'."


# splits.sh: nothing to do
# strip.sh: nothing to do

# tar.sh
msg_unpacking="Unpacking \`%1'."

msg_dunno_make_package="Don't know how to make \`%1'."

msg_not_supported_tar_impl="Not supported tar implementation."

# tools.sh

msg_make_tool_deprecated="Using \`%1' is deprecated to define a \'make' implementation."

msg_use_make_alternative="Use \`%1' instead."

msg_no_make_implementation_avail="No make implementation avaiable."

# TODO: sedi() strings

msg_use_enable_without_param="use_enable() used without a parameter."


# unpack.sh
msg_getting_sources="Getting sources of \`%1'."

msg_error_unpacking="Unpacking of \`%1' failed."

msg_build_failed="Building \`%1' failed."
