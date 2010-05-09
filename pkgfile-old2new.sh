#!/bin/bash

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
	depends="`cat $1 | grep "\# Depends on:" | sed -e "s|\# Depends on:||"`"
	depends="`echo $depends`"
	echo "packager=\"$packager\""
	echo "maintainer=\"$maintainer\""
	echo "url=\"$url\""
	echo "depends=($depends)"
	echo "name=$name"
	echo "version=$version"
	echo "release=$release"
	echo "source=(${source[@]})"
	type -a build | grep -v "$useless_type_return"
fi


