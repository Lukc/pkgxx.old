
# We could have a CHOST (and we should) without being cross-compiling.
if [[ -n "$CHOST" && "$CHOST" != "$MACHTYPE-$VENDOR-$OSTYPE" ]]; then
	export CROSS=true
	# We set the name of the most used tools to $CHOST-$GNU_TOOL.
	: ${CC:=${CHOST}-gcc}
	: ${CXX:=${CHOST}-g++}
	: ${CPP:=${CHOST}-cpp}
	: ${AS:=${CHOST}-as}
	: ${AR:=${CHOST}-ar}
	: ${LD:=${CHOST}-ld}
	: ${NM:=${CHOST}-nm}
	: ${RANLIB:=${CHOST}-ranlib}
	: ${STRIP:=${CHOST}-strip}
else
	: ${CBUILD:=$CHOST}
	: ${CTARGET:=$CHOST}
	export CROSS=false
fi

