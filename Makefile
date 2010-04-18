# pkg++, a fork of pkgutils's pkgmk

DESTDIR =
BINDIR = /usr/bin
SHAREDIR = /usr/share
ETCDIR = /etc

build: 
	@echo -e "Build ready, now you can make \033[01mmake install\033[00m"

install: 
	install -D -m0755 pkg++.in $(DESTDIR)$(BINDIR)/pkg++
	install -D -m0644 pkg++.conf $(DESTDIR)$(ETCDIR)/pkg++.conf
	@[ -d $(DESTDIR)$(SHAREDIR)/pkg++/includes ] || mkdir -pv $(DESTDIR)$(SHAREDIR)/pkg++/includes
	install -D -m0644 includes/* $(DESTDIR)$(SHAREDIR)/pkg++/includes/

# End of file
