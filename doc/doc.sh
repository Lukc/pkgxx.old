#!/usr/bin/env zsh

: ${markdown:=markdown}

sed -n -e "/^#</,/^#>/{s/^#.//;p}" $1 | \
	${markdown}

