# pkg++, a fork of pkgutils' pkgmk

DESTDIR =
BINDIR = /usr/bin
SHAREDIR = /usr/share
ETCDIR = /etc
MANDIR = $(SHAREDIR)/man

PKGXX_VERSION=0.7

build: 
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" pkg++.in > pkg++
	bash ./config_pkgxx.sh
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" pkg++.8.in > pkg++.8
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" pkg++.conf.5.in > pkg++.conf.5

config:
	install -D -m0644 pkg++.conf $(DESTDIR)$(ETCDIR)/pkg++.conf

man:
	install -D -m0644 pkg++.8 $(DESTDIR)$(MANDIR)/man8/pkg++.8
	install -D -m0644 pkg++.conf.5 $(DESTDIR)$(MANDIR)/man5/pkg++.conf.5

pkgxx:
	install -D -m0755 pkg++ $(DESTDIR)$(BINDIR)/pkg++
	@[ -d $(DESTDIR)$(SHAREDIR)/pkg++/includes ] || mkdir -pv $(DESTDIR)$(SHAREDIR)/pkg++/includes
	install -D -m0644 includes/* $(DESTDIR)$(SHAREDIR)/pkg++/includes/

update: build pkgxx man

install: build pkgxx config man

clean:
	rm pkg++
	rm pkg++.8
	rm pkg++.conf.5

# End of file
