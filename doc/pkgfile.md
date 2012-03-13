
<{ include doc.spp }>

<{ get work_in_progress }>

Last build: <{ system LANG=C date }>

# *Pkgfiles* reference #

This file is here to list all the functions and variables you can
both use, may it be by declaring them or using their value.

## Variables ##

### Undefined ###

Those are the variables you can define in a Pkgfile. Some of them
are mandatory, and others might help you avoiding problems.

#### ${builddepends[]} ####

Contains the dependencies needed at build-time but not at run-time.

Its content is similar to the one of ${depends[]}.

<{ get new_feature }>
<{ get recommended }>

#### $class ####

$class contains the “type” of software you packaged. The allowed
values are: `library`, `documentation`, `man-pages`, `headers` or
`sources`.

This variable is not mandatory, and not having set it is not a
problem.

Some modules may use additional values.

<{ get planned }>

#### ${contributors[]} ####

${contributors[]} is here to contain the names and mails of
all the previous contributors of the package or the port.

	# Example
	contributors=(
		"Some One <someone@upyum.com>"
		"The Packager <packager@pkgxx.org>"
	)

<{ get planned }>

#### ${depends[]} ####

You can write in this array the list of packages names your recipe
depends on.

As the package name of your dependencies change in most 
distributions or OSes, special variables are available.

The recommended default dependency names are the software names 
(lowercase, eg. `foo`).

The following example shows how to set two dependencies and how
to add the exact names of those dependencies on Debian.

	depends=(liba pkgb)
	
	if $Debian; then
		depends=(${depends//liba/liba-1.42})
		depends=(${depends//pkgb/b-pkg-foo-47})
	fi

<{ get recommended }>

To know exactly what special variables are available to determine 
the construction OS, look at 
[defaults/distributions](defaults/distributions.xhtml).

See also $builddepends and $usedepends.

#### ${iuse[]} ####

${iuse[]} is an array containing a list of [[useflags]] used in
the recipe.

	# Example
	iuse=(nls doc static)

<{ get recommended }>

For more informations about *useflags*, you should read the
[useflags reference](useflags.xhtml).

If you use custom flags, don’t forget to declare them.

#### $license ####

$license contains the license of the packaged software.

There is currently no standard or recommendation concerning how 
you should fill this form.

#### $maintainer ####

$maintainer contains the name of the port maintainer. The port
maintainer is the one doing the updates and corrections to
the port. It can differ from the $packager.

If you wrote a port, you should be $packager and $maintainer.
If you stop maintaining a port, you are still the $packager, 
but the $maintainer will change on any repository where your
port is copied and modified.

Its content is similar to the one of $packager.

#### $name ####

$name sets the name of the recipe.

<{ get mandatory }>

#### $packager ####

$packager defines the author of the port. Its format is
`name <[protocol:]user@host>`.

eg: `User <XMPP:me@my_server.org>` or `The One <one@mailserver.org>`

If you are a port maintainer, you should look instead at $maintainer.

<{ get recommended }>

#### $pkgname ####

$pkgname allows to redefine the package name (eg. libfoo instead
of foo). It can be used in includes and modules with $class 
without taking the risk to change $name and making the recipe
unusable.

#### $release ####

$release sets the version of the port. It must be an integer 
(1, 2, 3, ...). The special 0 value can be used if your port
is not complete (did never worked.).

<{ get mandatory }>

#### ${source[]} ####

${source[]} is an array than contains the URLs or URIs of sources,
patches, icons, etc. you need to build a software and its package.

<{ get mandatory }>

#### ${splits[]} ####

${splits[]} is an array containing a list of split names. Those 
names should contain only lowercase letters.

Each split inherits all values from the main package, except for
the name, which is by default the value given in ${splits[]}. To
redefine a value of a split, define &lt;split&gt;\_&lt;variable&gt;.
Each of these variables behave exactly as the main ones.

<{ get v0.14 }>

#### ${supports[]} ####

Supports contains a list of features the recipe offers. Its values
can be: `cross`, `autosplits`.

<{ get new_feature }>

#### $url ####

$url contains the URL of the web page (or web site) of the packaged 
software.

<{ get recommended }>

#### ${usedepends[]} ####

Contains the dependencies needed at run-time but not at build-time.

Its content is similar to the one of ${depends[]}.

<{ get new_feature }>
<{ get recommended }>

#### $version ####

$version sets the version of the software you are packaging.

<{ get mandatory }>

### Predefined ###

#### $PKG ####

This variable contains the name of the directory you must install
the built software in. Everything not in $PKG will not be part of
the package.

#### $SRC ####

$SRC is the directory in which sources are extracted and accessible.

## Functions ##

### Undefined ###

#### build() ####

build() must contain all the instructions required to build a
software and to install it in the good directory.

You might also want to look at the following variables/functions:

  * $PKG
  * pkgmake()

<{ get mandatory }>

#### post\_build() ####

post\_build() does exactly the same thing as build(), except it is run
just before.

It is mainly used when an [include file](include_files.xhtml) that
defines build() is used.

#### pre\_build() ####

pre\_build() does exactly the same thing as build(), except it is run
just before.

It is mainly used when an [include file](include_files.xhtml) that
definec build() is used.

### Predefined ###

#### pkgmake() ####

It is a wrapper to the first make implementation found. Most of time,
it just does a `make ${MAKE_OPTS[@]}`. It is recommended to use it
when your package does not need a particular make implementation.

