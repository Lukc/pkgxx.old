
# This Makefile is made to be used with bmake! (which is also known as
#+ NetBSD's make)

sed ?= sed

# Just find it yourself, there are implementations in every langage,
#+ even in AWK!
markdown ?= lua markdown.lua -n

# This one is a little more complicated to get. Just use 
#+ http://hg.youterm.com/spp. Everything is in the mercurial
#+ repository.
# If not available, an minimalist/few complete documentation will be
#+ built.
spp ?= spp

footer ?= footer.xhtml
header ?= header.xhtml

URL ?= http://pkgxx.org/doc

SRC != ls *.md | sed -e "s/\(.*\/\)*//;s/\.md$$//"
MOD != ls ../modules/*.zsh | sed -e "s|^../modules/||;s|\.zsh$$||"
INC != ls ../includes | sed -e "s|^../includes/||"

Q ?= @

all: sources modules includes
sources: ${SRC:S/$/.xhtml/}
modules: ${MOD:S/$/.xhtml/:S/^/modules\//} modules/index.xhtml
includes: ${INC:S/$/.xhtml/:S/^/includes\//} includes/index.xhtml

clean: sources-clean modules-clean includes-clean
sources-clean: ${SRC:S/$/.xhtml-clean/}
modules-clean: ${MOD:S/$/.xhtml-clean/:S/^/modules\//} modules/${header}-clean modules/${footer}-clean modules/index.xhtml-clean
includes-clean: ${INC:S/$/.xhtml-clean/:S/^/includes\//} includes/${header}-clean includes/${footer}-clean includes/index.xhtml-clean

.for src in ${SRC}
${src}.xhtml: ${src}.md ${header} ${footer}
	@echo " -- Building ${src}.xhtml."
	${Q}( \
		cat ${header} && \
		cat ${src}.md | sed -f links.sed | (${spp} -t spp || sed -e "s/<{.*}>//") | ${markdown} && \
		cat ${footer} \
	) > ${src}.xhtml || ( \
		echo " -- Building of ${src}.xhtml failed!" ; \
		rm ${src}.xhtml; \
	)

${src}.xhtml-clean:
	@echo " -- Removing ${src}.xhtml."
	${Q}rm -f ${src}.xhtml
.endfor

modules/:
	@mkdir -p modules/

modules/${header}: modules/
	@echo " -- Building modules/${header}."
	${Q}cat ${header} | sed -e "/<div id=\"submenu\">/,/<\/div>/{s/href=\"/href=\"..\//}" > modules/${header}
modules/${footer}: modules/
	@echo " -- Building modules/${footer}."
	${Q}cp ${footer} modules/${footer}

modules/${header}-clean:
	${Q}rm -f modules/${header}
modules/${footer}-clean:
	${Q}rm -f modules/${footer}

modules/index.xhtml: modules/${header} modules/${footer}
	@echo " -- Building modules/index.xhtml."
	${Q}( \
		cat modules/${header} && \
		echo -e "${MOD:S/$/\n/:S/^/  * /}" | sed -e "s/\( \* \)\([a-zA-Z0-9]*\)/\1<a href=\"${URL:S/\//\\\//g}\/modules\/\2.xhtml\">\2<\/a>/" | ${markdown} && \
		cat modules/${footer} \
	) > modules/index.xhtml
modules/index.xhtml-clean:
	@echo " -- Removing modules/index.xhtml."
	${Q}rm -f modules/index.xhtml

.for src in ${MOD}
modules/${src}.xhtml: doc.sh modules/ modules/${header} modules/${footer} ../modules/${src}.zsh
	@echo " -- Building modules/${src}.xhtml."
	${Q}( \
		cat modules/${header} && \
		markdown="${markdown}" sh doc.sh ../modules/${src}.zsh && \
		cat modules/${footer} \
	) > modules/${src}.xhtml || rm -f modules/${src}.xhtml

modules/${src}.xhtml-clean:
	@echo " -- Removing modules/${src}.xhtml."
	${Q}rm -f modules/${src}.xhtml
.endfor

includes/:
	@mkdir -p includes/

includes/${header}: includes/
	@echo " -- Building includes/${header}."
	${Q}cat ${header} | sed -e "/<div id=\"submenu\">/,/<\/div>/{s/href=\"/href=\"..\//}" > includes/${header}
includes/${footer}: includes/
	@echo " -- Building includes/${footer}."
	${Q}cp ${footer} includes/${footer}

includes/${header}-clean:
	${Q}rm -f includes/${header}
includes/${footer}-clean:
	${Q}rm -f includes/${footer}

includes/index.xhtml: includes/${header} includes/${footer}
	@echo " -- Building includes/index.xhtml."
	${Q}( \
		cat includes/${header} && \
		echo -e "${INC:S/$/\n/:S/^/  * /}" | sed -e "s/\( \* \)\([a-zA-Z0-9.\\-]*\)/\1<a href=\"${URL:S/\//\\\//g}\/includes\/\2.xhtml\">\2<\/a>/" | ${markdown} && \
		cat includes/${footer} \
	) > includes/index.xhtml
includes/index.xhtml-clean:
	@echo " -- Removing includes/index.xhtml."
	${Q}rm -f includes/index.xhtml


.for src in ${INC}
includes/${src}.xhtml: doc.sh includes/ includes/${header} includes/${footer} ../includes/${src}
	@echo " -- Building includes/${src}.xhtml."
	${Q}( \
		cat includes/${header} && \
		markdown="${markdown}" sh doc.sh ../includes/${src} && \
		cat includes/${footer} \
	) > includes/${src}.xhtml || rm -f includes/${src}.xhtml

includes/${src}.xhtml-clean:
	@echo " -- Removing includes/${src}.xhtml."
	${Q}rm -f includes/${src}.xhtml
.endfor

# spp is needed to build some of the documentation.
# As very few distros have packaged it with my own personal changes, here
#+ is a bootstrap rule. Those changes are vital for real-world use of spp
#+ (in my humble opinion), and more important, are vital to build pkg++’
#+ documentation.

spp: spp.git
	@echo " -- Building spp."
	${Q}cd spp.git; make CFLAGS="-O3 -march=native"
	${Q}cp spp.git/spp spp

spp.git:
	@echo " -- Cloning spp’s git repository."
	${Q}git clone git://upyum.com/spp spp.git


