#!/usr/bin/env bash

VERSION="$1"

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

configure() {
	sed -e "s|@ARCH@|$ARCH|" $1 > $1.tmp
	mv $1.tmp $1
	sed -e "s|@KERNEL@|$KERNEL|" $1 > $1.tmp
	mv $1.tmp $1
	sed -e "s|@VERSION@|$VERSION|" $1 > $1.tmp
	mv $1.tmp $1
}

echo "#!/usr/bin/env bash" > "pkg++"
gcc -E -x c -P -C "pkg++.in" >> "pkg++"
cp "pkg++.conf.in" "pkg++.conf"
configure pkg++

