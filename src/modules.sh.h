
#define pkg_manager_add(__PM_NAME) \
	PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} __PM_NAME)

#define pkg_manager_noarch(__PM_NAME) \
	PKGMK_PM_NOARCH_SUPPORT=(${PKGMK_PM_NOARCH_SUPPORT[@]} __PM_NAME)

#define scm_add(__SCM_NAME) \
	PKGMK_SCMS=(${PKGMK_SCMS[@]} __SCM_NAME)

#define untar_add(__UNTAR_NAME) \
	PKGMK_UNTAR_TOOLS=(${PKGMK_UNTAR_TOOLS[@]} __UNTAR_NAME)

