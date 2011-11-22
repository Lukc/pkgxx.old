
<{ include doc.spp }>

<{ get work_in_progress }>

This file is here to help you writting modules.

# TODO list #

  * package managers
  * extraction/decompression tools
  * non-vcs download tools
  * recipes

# Types of modules

## Version Control System module ##

You need to declare at least two functions:

  * vcs:clone()
  * vcs:pull()

For an example, look at the modules/git.zsh file of pkg++’ sources.

### Reference ###

#### vcs:clone() ####

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

This function gets the revision number of the repository $1, and 
prints it on *stdout*.

<{ get new_feature }>
<{ get module_recommended }>

