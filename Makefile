# pkg++, a fork of pkgutils's pkgmk

DESTDIR =
BINDIR = /usr/bin
ETCDIR = /etc

build: 
	@echo -e "Build ready, now you can make \033[01mmake install\033[00m"

install: 
	install -D -m0755 pkg++ $(DESTDIR)$(BINDIR)/pkg++
	install -D -m0644 pkg++.conf $(DESTDIR)$(ETCDIR)/pkg++.conf

# End of file
