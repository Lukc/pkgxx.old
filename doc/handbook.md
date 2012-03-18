
** This handbook is still being written. **

# FIXME/TODO list #

Are critically to be completed:

  * Introduction (What is pkg++?, Why do people use pkg++?)
  * Say somewhere this handbook is for pkg++ >= 0.11.3 (maybe even 0.12?).
  * Creating new ports −> Divide into smaller parts.
  * Well, everything.

# Introduction #

## Note from Lukc ##

In the hope I won’t be the only author…

## What is pkg++? ##

pkg++ is a tool to build packages, but not any tool, because it can build
any package.

## Why do people use pkg++? ##

pkg++ is very easy to use, and it is very easy to build packages with it.
It tries to be as simple as possible in every aspect.

# Installing pkg++ #

To install pkg++, you need to have both build dependencies and runtime 
dependencies. To have them both, you need to install the gcc[1] and the zsh
packages from your distributions.

## Tarballs versions ##

The tarballs versions are the more stable versions. There are always two of 
them available, one of the development branch and the other from the stable 
branch. The one from the stable branch tends to be outdated really quickly, 
whereas the one from the development branch can break more often. It is to 
you to decide, but, generally, it is recommended to use the stable release 
and in case of problem (important failures, too modern recipes, etc.) to 
use the “active” one.

Usually, a `wget http://devel.upyum.com/pkgxx/pkg++-<version>.tar.xz`
will get you the sources.

To extract them, you need a tar implementation. You usually have a tool `tar`
on your system. A `tar xvJf pkg++-<version>.tar.xz` will extract a 
pkg++-&lt;version&gt; folder in your current directory.

Move to the extracted directory and use the script `configure`[2] at it’s root
to configure the construction of pkg++. You can specify a prefix other than 
`/usr` with the option `--prefix=your_prefix` and a system configuration
directory other than `/etc` with `--sysconfdir=your_sysconfdir`.

	cd pkg++-<version>
	./configure \
		--prefix=/usr \
		--sysconfdir=/etc

Then, build and install it with make.

	make
	make install

If you are updating, you might prefer to run `make update` instead of 
`make install`, as this command will not erase any previously installed
configuration you might have edited.

## Git version ##

Using the git version is only for advanced users! If you are a pkg++ beginner,
just use the development tarball (see the previous paragraph).

	git clone git://upyum.com/pkgxx

# Using pkg++ #

## Vocabulary ##

## Basics ##

To use pkg++ you need a [pkgfile](pkgfile.xhtml), or another type of 
supported recipe if you only have that. Just run pkg++ when you are in
the Pkgfile directory, or `pkg++ <recipe>` if you are using something 
other than a Pkgfile.

[Pkgfiles](pkgfile.xhtml) are always recommended before the use of any kind 
of other recipe, as it is the native recipe format of pkg++.

## Advanced use ##

pkg++ supports a large variety of options and parameters to allow it’s 
behavior. Additionally, you can write [defaults](Defaults files.xhtml)
files and [profiles](Profiles.xhtml) to alter it’s behavior more 
permanently.

# Creating new ports #

## The simpler port ever imaginable ##

You have learned to use pkg++, and are now interesting in building your
own packages, from your own recipes, but you do not know where to start.
This part of the handbook is here to guide you in new ports creation.

If you need a particular tool or a precise description of a precise and
never-used variable or function, you may want to look instead at the 
[Pkgfile reference](pkgfile.xhtml).

So, to create a new port, we first need to create a directory of the same
name as the final package name, and to then add a file called `Pkgfile`
in it.

	mkdir my_port
	cd my_port
	touch Pkgfile

Then use your favorite text editor to add content into that file. The first
four lines you need to add are the following:

	name=my_port
	version=my_port_version
	release=1
	source=(my_url/my_archive)

$name must be the name of your port, $version the version of your port and
$source is an array containing the URLs of all the sources (as much tarballs
as patches) that you need to build your package.

The $release variable is a little more subtle. It is the version of you 
recipe (and package, eventually) itself. It means that when you release 
your port, your $release should be 1. If you modify it and re-release it,
your $release should then be 2. And if $version is increased, $release
should be reset to 1. $release must be *always* an integer.

After these four variables, you need to declare the build() function. The
build() function defines all the instructions required to configure, build
and install a software.

It seems very simple, but you have to remind that build() must install 
anything in $PKG, and not on the root of your system! Also, the sources
are always already extracted when the build() content is executed, but
build() is executed from the directory the sources where extracted in, 
not in the extracted directory.

The following example shows what a simple build() contains. It should 
also help you to understand the details quoted sooner.

	build() {
		cd $name-$version
		./configure --prefix=/usr
		make
		make DESTDIR="$PKG"
	}

This build() is not portable (will not meet the guidelines of any 
distribution) and is not very flexible, but it should build a correct
package. If you did not understood what is written before this sentence, 
reread it again and again until you understand, as the next of this 
tutorial details it.

## A better port ##

If you are here, you already made a simple port and its recipe. Now, we
will see how we can, first, enhance a recipe to make it more portable, and
then how to make it more flexible.

To illustrate, imagine a package that can be built only on MirBSD (which
most of you never heard about) but not on any other OS, and certainly not
yours. This would be a very unportable package, and would be also very sad.
It is true some softwares are just not buildable on OS others than their
native’s one, but Pkgfiles should also manage that case, to tell the user
what cannot be done instead of letting him try and fail.

A flexible recipe would be a package that can adapt to the user needs. 
Another image would be a recipe that builds a software for GNOME every
time you build the package, but of a software that can be built for KDE
or Enlightenment as well. That kind of recipe would be very inflexible.

### Portability ###

The first (and probably easiest) thing to do to make a recipe a little more
portable is to use variables instead of hard coded directories. The two 
following code blocks will have the same effect on most OSes. However, 
one is portable and not the other.

	./configure --prefix=/usr

	./configure --prefix=$prefix

### Flexibility ###

# Distributing ports #

# Other tools #

## `ports` ##

# Footnotes #

\[1\]: In fact, you just need a C PreProcessor (cpp). The one from any other
compiler should work, but none have been tested as of now.

\[2\]: This is not an autotools script. A bmake Makefile is still under development.

