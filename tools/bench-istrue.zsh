#!/usr/bin/env zsh

# SLOW TEST:
# In order to check real efficiency, the tests are made thousands times,
# which will make the benchmark a little slow.

export PATH="$PATH:.:/usr/libexec/pkg++/"

istrue() {
	if has "$1" true TRUE y yes 1; then
		return 0
	elif has "$1" false FALSE n no 0; then
		return 1
	else
		return 2
	fi
}

echo "--> Running istrue benchmark"

d1="$(date +%s.%N)"
for i in {1..9999}; do
	istrue 0
done

d2="$(date +%s.%N)"
echo "zsh: $(($d2-$d1))s"

d3="$(date +%s.%N)"
for i in {1..9999}; do
	./istrue 0
done

d4="$(date +%s.%N)"
echo "c:   $(($d4-$d3))s"

