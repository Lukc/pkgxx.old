
# This Makefile is made to be used with bmake! (which is also known as
#+ NetBSD's make)

sed ?= sed

# Just find it yourself, there are implementations in every langage,
#+ even in AWK!
markdown ?= lua markdown.lua -n

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
		cat ${src}.md | sed -f links.sed | ${markdown} && \
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

