# pkg++, a fork of pkgutils' pkgmk

DESTDIR =
BINDIR = /usr/bin
SHAREDIR = /usr/share
ETCDIR = /etc
MANDIR = $(SHAREDIR)/man

PKGXX_VERSION=0.7.2

build: 
	bash ./config_pkgxx.sh ${PKGXX_VERSION}
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" pkg++.8.in > pkg++.8
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" pkg++.conf.5.in > pkg++.conf.5

config:
	@[ -d $(DESTDIR)$(ETCDIR) ] || mkdir -pv $(DESTDIR)$(ETCDIR)
	install -m0644 pkg++.conf $(DESTDIR)$(ETCDIR)/pkg++.conf

man:
	@[ -d $(DESTDIR)$(MANDIR) ] || mkdir -pv $(DESTDIR)$(MANDIR)
	install -m0644 pkg++.8 $(DESTDIR)$(MANDIR)/man8/pkg++.8
	install -m0644 pkg++.conf.5 $(DESTDIR)$(MANDIR)/man5/pkg++.conf.5

pkgxx:
	@[ -d $(DESTDIR)$(BINDIR) ] || mkdir -pv $(DESTDIR)$(BINDIR)
	install -m0755 pkg++ $(DESTDIR)$(BINDIR)/pkg++
	@[ -d $(DESTDIR)$(SHAREDIR)/pkg++/includes ] || mkdir -pv $(DESTDIR)$(SHAREDIR)/pkg++/includes
	install -m0644 includes/* $(DESTDIR)$(SHAREDIR)/pkg++/includes/

update: build pkgxx man

install: build pkgxx config man

clean:
	rm -rf pkg++
	rm -rf pkg++.8
	rm -rf pkg++.conf
	rm -rf pkg++.conf.5

pkgfiles:
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" Pkgfile.Crux.in > Pkgfile.Crux
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" Pkgfile.Nutritive.in > Pkgfile.Nutritive
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" Pkgfile.NuTyX.in > Pkgfile.NuTyX
	sed -e "s|@VERSION@|${PKGXX_VERSION}|g" Pkgfile.devel.in > Pkgfile.devel

# End of file
