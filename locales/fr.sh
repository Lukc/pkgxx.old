
# Common
msg_func_may_have_failed="\`%1()' a dû échoué."
 msg_file_is_not_directory="\`%1' existe déjà et n'est pas un répertoire."
msg_file_not_readable="\`%1' ne peut être lu."
msg_could_not_cd="Ne peut pas être déplacé vers \`%1'."

# main.sh.c
msg_interrupted="Interrompu."

msg_control_sum_updated="%1 mis à jour."

msg_extracting_sources="Extraction des sources du paquet \`%1'."

msg_not_up_to_date="Le paquet \`%1' n'est pas à jour."
msg_up_to_date="Le paquet \`%1' est à jour."

msg_unavailable_profile="Le profil demandé (\`%1') n'est pas disponible."
msg_file_not_found="Fichier \`%1' non trouvé."
msg_no_recipe_format="Aucun moyen d'analyser la recette n'a été trouvé. Veuillez installer au moins un module."
msg_cannot_be_parsed="\`%1' ne peut être analysé."
msg_missing_dependencies="Toutes les dépendances n'ont pu être trouvées sur votre système."

# build.sh.c
msg_should_build_as_root="Les paquets devraient être construits en tant que super-utilisateur."

msg_building_package="Construction de \`%1'%2."
msg_build_success="La construction de \`%1' a été effectuée avec succès."
msg_build_fail="La construction de \`%1' a échoué."

msg_testing="Test de \`%1'."
msg_tests_success="\`%1' a été correctement testé." # fuzzy
msg_tests_fail="Les tests de \`%1' ont échhoué."

msg_log_avail_at="Journal de construction disponible ici : \`%1'"

msg_bad_autosplit="Automatic split \`%1' is not available."

msg_build_result="Résultat de la construction :"

# signatures.sh.c
msg_signing="Signature de \`%1'."

# downloads.sh.c
msg_getting="Téléchargement de \`%1'."
msg_cannot_fetch_through_protocol="pkg++ ne peut récupérer les sources via \`%1'."
msg_already_u2d="\`%1' est déjà à jour."
msg_localfile_not_found="Le fichier source \`%1' est introuvable (ne peut être téléchargé, adresse de téléchargement non spécifiée)."
msg_localfile_not_downloaded="Fichier source \`%1' introuvable (utilisez l'option -d pour le télécharger)."

# clean.sh
msg_removing_local_filename="Suppression de \`%1'."

# checksums.sh
msg_algo_notfound="\`%1' non trouvé."

msg_create_algo="\`%1' non trouvé, création d'un nouveau."

msg_algo_mismatch="\`%1' différence trouvée : "

msg_algo_notok="\`%1' pas OK."

msg_build_failed="La construction de \`%1' a échoué."

msg_algo_ok="\`%1' OK."


# footprints.sh
msg_new_files_found="Nouveaux fichiers trouvés : "

msg_footprint_mismatch="Footprint mismatch found:"

msg_create_footprint="Footprint not found, creating new."

msg_package_not_found="Le paquet \`%1' n'a pas été trouvé."

msg_unable_update_footprint="Unable to update footprint. File \`%1' not found."

msg_footprint_updated="Footprint updated."


# includes.sh
msg_error_while_including="Une erreur s'est produite lors de l'inclusion de \`%1'."

msg_include_file_notfound="Le fichier d'inclusion \`%1' n'a pas été trouvé."


# install.sh
msg_installing_info="Installation de \`%1'."

msg_success_install="L'installation de \`%1' s'est correctement déroulée."

msg_failure_install="L'installation de \`%1' a échoué."

# interactions.sh
# TODO: add string for the question
msg_no_useflag_info="There is no available information about use flag \`%1'."


# opts.sh
# TODO: translate print_help() content
# TODO: translate whole file


# recursive.sh
msg_entering_dir="Entrée dans le répertoire \`%1'."

msg_leaving_dir="Sortie du répertoire \`%1'."


# splits.sh: nothing to do
# strip.sh: nothing to do

# tar.sh
msg_unpacking="Décompression de \`%1'."

msg_dunno_make_package="Ne sais pas comment construire \`%1'."

msg_not_supported_tar_impl="Not supported tar implementation."

# tools.sh

msg_make_tool_deprecated="Using \`%1' is deprecated to define a \'make' implementation."

msg_use_make_alternative="Utilisez \`%1' à la place."

msg_no_make_implementation_avail="No make implementation available."

# TODO: sedi() strings

msg_use_enable_without_param="use_enable() utilisé sans paramêtre."


# unpack.sh
msg_getting_sources="Téléchargement des sources de \`%1'."

msg_error_unpacking="La décompression de \`%1' a échoué."

msg_build_failed="La construction de \`%1' a échoué."
