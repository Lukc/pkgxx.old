
<{ include doc.spp }>

<{ get work_in_progress }>

This file is here to help you writting modules.

<!--
# TODO list #

  * extraction/decompression tools
  * non-vcs download tools
  * recipes (once fixed, working and tested)
-->

# Types of modules

## Version Control System module ##

You need to declare at least two functions:

  * vcs:clone()
  * vcs:pull()

For example, look at the modules/git.zsh file of pkg++’ sources.

### Reference ###

#### vcs:clone() ####

`vcs:clone <repository>`

This function must clone (or checkout) a repository to a directory
which name is $name.

You should assure yourself that things like SSL will not block the 
checkout of the repository.

<{ get module_required }>

#### vcs:pull() ####

This function must update a cloned repository. It is normally ran
from the inside of the repository, so no need to use `cd`.

It should also check whether the update was successful (new files
were added, modified or removed) or not (the remote repository did 
not changed since the last pull or clone).

<{ get module_required }>

#### vcs:revision() ####

`vcs:revision <repository>`

This function gets the revision number of the repository $1, and 
prints it on `stdout`.

<{ get new_feature }>
<{ get module_recommended }>

## Package managers ##

### Reference ###

#### pkg:target() ####

`pkg:target()` is a function that returns a package name on stdout.
It is called aften the recipe has been parsed, and so can use
$name, $version, $revision, etc.

The package name must be absolute and must be prefixed by
`$PKGMK_PACKAGE_DIR`.

<{ get mandatory }>

#### pkg:checkdeps() ####

The content of this function is not defined yet. It more of a
test as of  0.12.

<{ get new_feature }>

#### pkg:build() ####

`pkg:build()` is the heart of a package manager module. Its role
is to build a package.

The content of what is to be packaged is available in $PKG.

Absolute path and name of the package are stored in $TARGET.
So, once built, $TARGET must match your package on the local
filesystem.

<{ get mandatory }>

#### pkg:footprint() ####

<{ get old_feature }>

#### pkg:install() ####

This function should print the command required to install 
$TARGET on $PKGMK\_INSTALL\_ROOT or `/`, as 
$PKGMK\_INSTALL\_ROOT is not always available.

<!-- vim:set syntax=markdown: -->
