
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

SRC != ls *.md | sed -e "s/\(.*\/\)*//;s/\.md$$//"

Q ?= @

all: sources modules
sources: ${SRC:S/$/.xhtml/}
modules: ${MOD}
# FIXME: We will extract documentation/informations directly from the modules
#        source code/comments, in the future. (or not)

clean: sources-clean
sources-clean: ${SRC:S/$/.xhtml-clean/}

.for src in ${SRC}
${src}.xhtml:
	@echo " -- Building ${src}.xhtml."
	${Q}( \
		cat ${header} && \
		cat ${src}.md | sed -f links.sed | (${spp} -t spp || sed -e "s/<{.*}>//") | ${markdown} && \
		cat ${footer} \
	) > ${src}.xhtml || ( \
		echo "-- Building of ${src}.xhtml failed!" ; \
		rm ${src}.xhtml; \
	)
.endfor

.for src in ${SRC}
${src}.xhtml-clean: ${src}.xhtml
	@echo " -- Removing ${src}.xhtml."
	${Q}rm -f ${src}.xhtml
.endfor

# spp is needed to build some of the documentation.
# As very few distros have packaged it, here is a bootstrap rule.

spp: spp.hg
	@echo " -- Building spp."
	${Q}cd spp.hg; make CFLAGS="-O3 -march=native"
	${Q}cp spp.hg/spp spp

spp.hg:
	@echo " -- Cloning spp’s mercurial repository."
	${Q}hg clone http://hg.youterm.com/spp spp.hg


