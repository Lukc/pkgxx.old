
# *useflags* #

This page gives documentation about useflags. It explains what it is
and how to use it.

A use flag is a keyword that represents the support of a feature or
of a dependency. For example, a server does not need X, so its use
flag “X” will be disabled. On the other hand, a destkop with GNOME
or KDE will need X, and so will enable the “X” use flag during the
construction of any package that can use it.

The last example was not the one you will encounter every day. The 
one you will encounter every day is the one about choosing whether
or not you have to build an application with GTk or Qt libraries, 
whether you have to enable GNOME or KDE tools, which languages
you want to have bindings for, and so on. You can either edit the
recipe of the package yourself, which is what you *have* to do on
most OS, or you can edit your configuration and use a recipe
that takes care of your choices.

## On the user side ##

### Configuration ###

For the user, use flags are very simple to manage. You just need to
edit your configuration to add a `USE=()` variable, containing the
keywords you want to use.

Enabled flag are written either `flag` or `+flag`. If you want to 
explicitly disable a flag, that is possible by using the `-flag` 
syntax.

Your *USE* variable can take multiple lines. Just look at the 
example below.

	USE=(
		nls zsh awk
		-python -java +ruby
		X gnome -qt -kde
	)

You can add flags to your *USE* variable with 
`USE=(${USE[@]} new flags`. If you want to display all you useflags
from a script, just `echo ${USE[@]}`.

### Default *useflags* ###

Most useflags and their effect on a package depend on the recipe
you are using. However, some of them (well, a few of them) are
generic and their keywords and functions are decided by the pkg++
team.

*A script or viewer might be usefull, but none is available for 
now. One will surely appear in a near future.*

Those flags are all declared in /etc/pkg++/defaults/uses.

### Build ###

When you build a package that can manage at least one useflag, they
will be displayed at the beginning of the build process. The syntax
of the flags shown is the following:

  * `flag`: Flag is enabled.
  * `-flag`: Flag is disabled.
  * `~flag`: Flag is not enabled, nor disabled, but just “unset”.
    It will be considered as disabled by most recipes, but some of
    them may use the subtle difference between unset and disabled.

If you are in interactive mode (`pkg++ -ia`), pkg++ will ask you 
about any useflag in the recipe if you want to enable it or not.

## On the packager side ##

This part of this documentation is here to guide packagers in their
learning of how-to-master-useflags-from-the-inside. If you are a 
mere user who just want to get things to work as you want them to
work, this part will learn you very little.

If you are a packager, it is, on the contrary, recommended that you
read this file.

*If you are a packager, you will have to wait until I complete
this documentation.*

### use() ###

use() is one of the most useful functions. It allows checking for
*useflags* activation/deactivation status.

