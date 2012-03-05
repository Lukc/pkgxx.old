#!/usr/bin/env zsh

: ${markdown:=markdown}

sed -n -e "/^[ 	]*#</,/^[ 	]*#>/{
	s/^[ 	]*#//
	s/^[ ><]//
	p
}" $1 | \
	${markdown}

