
<{ include doc.spp }>

<{ get work_in_progress }>

# *Pkgfiles* reference #

This file is here to list all the functions and variables you can
both use, may it be by declaring them or using their value.

## Variables ##

### Undefined ###

Those are the variables you can define in a Pkgfile. Some of them
are mandatory, and others might help you avoiding problems.

#### $class ####

$class contains the “type” of software you packaged. The allowed
values are: "library".

<{ get planned }>

#### ${iuse[]} ####

${iuse[]} is an array containing a list of [[useflags]] used in
the recipe.

<{ get recommended }>

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
but the $maintainer will change on any repository where you
port is copied and modified.

#### $name ####

$name sets the name of the recipe.

<{ get mandatory }>

#### $packager ####

$packager defines the author of the port. Its format is
`name <[protocol:]user@host>`.

eg: `User <XMPP:me@my_server.org>` or `The One <one@mailserver.org>`

If you are a port maintainer, you should look instead at $maintainer.

<{ get recommended }>

#### $release ####

$release sets the version of the port. It must be an integer 
(1, 2, 3, ...). The special 0 value can be used if your port
is not complete (did never worked.).

<{ get mandatory }>

#### ${source[]} ####

${source[]} is an array than contains the URLs or URIs of sources,
patches, icons, etc. you need to build a software and its package.

<{ get mandatory }>

#### $url ####

$url contains the URL of the web page (or web site) of the packaged 
software.

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

post_build() does exactly the same thing as build(), except it is run
just before.

It is mainly used when an [include file](include_files.xhtml) that
defines build() is used.

#### pre\_build() ####

pre_build() does exactly the same thing as build(), except it is run
just before.

It is mainly used when an [include file](include_files.xhtml) that
definec build() is used.

### Predefined ###

#### pkgmake() ####

It is a wrapper to the first make implementation found. Most of time,
it just does a `make ${MAKE_OPTS[@]}`. It is recommended to use it
when your package does not need a particular make implementation.

