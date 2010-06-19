# pkg++, a fork of pkgutils's pkgmk

DESTDIR =
BINDIR = /usr/bin
SHAREDIR = /usr/share
ETCDIR = /etc
MANDIR = $(SHAREDIR)/man

build: 
	@echo -e "Build ready, now you can make \033[01mmake install\033[00m"

config:
	install -D -m0644 pkg++.conf $(DESTDIR)$(ETCDIR)/pkg++.conf

man:
	install -D -m0644 pkg++.8.in $(DESTDIR)$(MANDIR)/man8/pkg++.8

pkgxx:
	install -D -m0755 pkg++.in $(DESTDIR)$(BINDIR)/pkg++
	@[ -d $(DESTDIR)$(SHAREDIR)/pkg++/includes ] || mkdir -pv $(DESTDIR)$(SHAREDIR)/pkg++/includes
	install -D -m0644 includes/* $(DESTDIR)$(SHAREDIR)/pkg++/includes/

update: pkgxx man

install: pkgxx config man

# End of file
