#!/bin/bash

arch() {
	arch="$(uname -m)"
	case ${arch} in
		alpha|ia64|m68k|ppc|ppc64|x86_64) ;;
		arm*) arch=arm ;;
		i?86) arch=x86 ;;
		mips*) arch=mips ;;
		parisc*) arch=hppa ;;
		"Power Macintosh") arch=ppc ;;
		s390*) arch=s390 ;;
		sh*) arch=sh ;;
		sparc*) arch=sparc ;;
	esac
	echo ${arch}
}

kernel() {
	kernel="$(uname -s)"
	case ${kernel} in
		Linux|FreeBSD) ;;
	esac
	echo ${kernel}
}

: ${ARCH:="`arch`"}
: ${KERNEL:="`kernel`"}

sed -i -e "s|@ARCH@|$ARCH|" $1
sed -i -e "s|@KERNEL@|$KERNEL|" $1

