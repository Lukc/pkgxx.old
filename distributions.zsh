#!/usr/bin/env zsh

checking_for_bin () {
	echo -n "checking for $1... "
	for i in `echo "$PATH" | sed "s|:| |g"`; do
		if [[ -x $i/$1 ]]; then
			echo "yes"
			return 0
		fi
	done
	echo "no"
	return 1
}

DISTRIBUTIONS=(
	# We put here only the most known distributions.
	Crux Arch Slackware Frugalware   # KISS
	Fedora Mandriva                  # RPM-based
	Debian Ubuntu                    # dpkg/Debian-based
	FreeBSD OpenBSD NetBSD           # *BSD
	Cygwin                           # Problems.
)

if [[ -e /usr/bin/crux ]]; then
	: ${DISTRIBUTION:="Crux"}
elif [[ -e /etc/frugalware-release ]]; then
	: ${DISTRIBUTION:="Frugalware"}
elif [[ -e /etc/fedora-release ]]; then
	: ${DISTRIBUTION:="Fedora"}
elif [[ -e /etc/mandriva-release ]]; then
	: ${DISTRIBUTION:="Mandriva"}
elif [[ -e /etc/arch-release ]]; then
	: ${DISTRIBUTION:="Arch"}
elif [[ $PACKAGE_MANAGER = pkgtools ]]; then
	: ${DISTRIBUTION:="Slackware"}
elif [[ -e /etc/lsb-release ]]; then
	if . /etc/lsb-release; then
		# Should cover Ubuntu and Debian, at least.
		: ${DISTRIBUTION:="$DISTRIB_ID"}
		: ${DISTRIBUTION_VERSION:="$DISTRIB_RELEASE"}
	else
		echo "  /etc/lsb-release could not be parsed." >&2
	fi
elif [[ "$KERNEL" =~ (Free|Net|Open)BSD ]]; then
	# Both should be valid.
	: ${DISTRIBUTION:="$KERNEL"}
else
	: ${DISTRIBUTION:="Unknown"}
fi

case $DISTRIBUTION in
	Ubuntu)   FAMILY=Debian ;;
	*)        FAMILY=$DISTRIBUTION ;;
esac

[[ -n "$DISTRIBUTION" ]] && echo "distribution=\"$DISTRIBUTION\""
[[ -n "$FAMILY" ]]       && echo "distribution_family=\"$FAMILY\""

for d in ${DISTRIBUTIONS[@]}; do
	if [[ "$DISTRIBUTION" = "$d" ]]; then
		echo "$d=true"
	else
		if [[ "$FAMILY" = "$d" ]]; then
			echo "$d=true"
		else
			echo "$d=false"
		fi
	fi
done

