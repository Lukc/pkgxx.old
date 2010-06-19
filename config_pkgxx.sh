#!/bin/bash

arch() {
	arch="$(uname -m)"
	case ${arch} in
		alpha|ia64|m68k|ppc|ppc64) ;;
		arm*) arch=arm ;;
		i?86) arch=x86 ;;
		mips*) arch=mips ;;
		parisc*) arch=hppa ;;
		"Power Macintosh") arch=ppc ;;
		s390*) arch=s390 ;;
		sh*) arch=sh ;;
		sparc*) arch=sparc ;;
		x86_64) arch=x86_64 ;;
	esac
	echo ${arch}
}

: ${ARCH:="`arch`"}
: ${KERNEL:="`uname -s`"}

sed -i -e "s|@ARCH@|$ARCH|" pkg++
sed -i -e "s|@KERNEL@|$KERNEL|" pkg++

