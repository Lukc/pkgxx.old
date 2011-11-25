
# We could have a CHOST (and we should) without being cross-compiling.
# We also check that our recipe *can* cross-compile.
if [[ -n "$CHOST" && "$CHOST" != "$MACHTYPE-$VENDOR-$OSTYPE" ]] && USE=(${supports[@]}) use cross; then
	export cross=true
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
	: ${CHOST:=$MACHTYPE-$VENDOR-$OSTYPE}
	: ${CBUILD:=$CHOST}
	: ${CTARGET:=$CHOST}
	export cross=false
fi

if [[ -n "$CROSS" ]]; then
	# DEPRECATION: Remove for 0.14.
	export CROSS=$cross
	warning
	warning "\$CROSS is deprecated! If you read this message, you have"
	warning "  exported it either in your configuration or in a defaults"
	warning "  file. If you never exported it, then you should remove"
	warning "  the file pkg++/defaults/cross of your system configuration"
	warning "  directory (usually /etc or /usr/etc)."
	warning
	warning "If you are lazy, a 'rm /etc/pkg++/defaults/cross' should work."
	warning
fi

