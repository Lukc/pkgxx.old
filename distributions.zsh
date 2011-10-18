#!/usr/bin/env zsh

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
elif [[ $PACKAGE_MANAGER = dpkg ]] && checking_for_bin "apt-get" &> /dev/null;
then
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

