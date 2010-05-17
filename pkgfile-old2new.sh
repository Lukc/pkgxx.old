#!/bin/bash

#############################################################################
#  Copyright © 2010 Luka Vandervelden                                       #
#  Some rights reserved.                                                    #
#                                                                           #
# Redistribution and use in source and binary forms, with or without        #
# modification, are permitted provided that the following conditions        #
# are met:                                                                  #
# 1. Redistributions of source code must retain the above copyright         #
#    notice, this list of conditions and the following disclaimer           #
#    in this position and unchanged.                                        #
# 2. Redistributions in binary form must reproduce the above copyright      #
#    notice, this list of conditions and the following disclaimer in the    #
#    documentation and/or other materials provided with the distribution.   #
#                                                                           #
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR   #
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES #
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.   #
# IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,       #
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT  #
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, #
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     #
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       #
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF  #
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.         #
#############################################################################

die(){
	echo "$@" >&2
	exit 1
}

if [[ -n "$1" ]]; then
	[[ -e $1 ]] && \
	[[ -r $1 ]] || \
	die "$1 doesn't exist or is not readable."
	. $1
	useless_type_return="`type -a build | tac | tail -n 1`"
	packager="`cat $1 | grep "\# Packager:" | sed -e "s|\# Packager:||"`"
	packager="`echo $packager`"
	maintainer="`cat $1 | grep "\# Maintainer:" | sed -e "s|\# Maintainer:||"`"
	maintainer="`echo $maintainer`"
	url="`cat $1 | grep "\# URL:" | sed -e "s|\# URL:||"`"
	url="`echo $url`"
	depends="`cat $1 | grep "\# Depends on:" | sed -e "s|\# Depends on:||" | sed -e "s|,| |g"`"
	depends="`echo $depends`"
	echo "packager=\"$packager\""
	echo "maintainer=\"$maintainer\""
	echo "url=\"$url\""
	echo "depends=($depends)"
	echo -e "\nname=$name"
	echo "version=$version"
	echo "release=$release"
	echo "source=(${source[@]})"
	build_content="`type -a build | grep -v "$useless_type_return"`"
	if [[ "$build_content" = "build () 
{ 
    cd \$name-\$version;
    ./configure --prefix=/usr;
    make;
    make DESTDIR=\$PKG install
}" ]]; then
		echo -e "\nincludes=(autotools)"
	elif [[ "$build_content" = "build () 
{ 
    cd \$name-\$version;
    cmake -DPREFIX=/usr;
    make;
    make DESTDIR=\$PKG install
}" ]]; then
		echo -e "\nincludes=(cmake)"
	elif [[ "$build_content" = "build () 
{ 
    cd \$name-\$version;
    python setup.py install --root=\$PKG
}" ]]; then
		echo -e "\nincludes=(python)"
	else
		type -a build | grep -v "$useless_type_return"
	fi
fi


