#!/usr/bin/env zsh

checking_for_bin () {
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
	Crux Frugalware Fedora Mandriva Arch Slackware Debian FreeBSD
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
elif [[ $PACKAGE_MANAGER = dpkg ]] && checking_for_bin "apt-get";
then
	# FIXME: Debian is not the only distribution with dpkg…
	: ${DISTRIBUTION:="Debian"}
elif [[ "$KERNEL" = FreeBSD ]]; then
	: ${DISTRIBUTION:="FreeBSD"}
else
	: ${DISTRIBUTION:="Unknown"}
fi


for d in ${DISTRIBUTIONS[@]}; do
	if [[ "$DISTRIBUTION" = "$d" ]]; then
		echo "distribution=\"$DISTRIBUTION\""
		echo "distribution_family=\"$DISTRIBUTION\""
		echo "$d=true"
	else
		echo "$d=false"
	fi
done

