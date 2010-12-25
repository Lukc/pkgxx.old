
#define pkg_manager_add(__PM_NAME) \
	PKGMK_PACKAGE_MANAGERS=(${PKGMK_PACKAGE_MANAGERS[@]} __PM_NAME)

#define scm_add(__SCM_NAME) \
	PKGMK_SCMS=(${PKGMK_SCMS[@]} __SCM_NAME)

