# This file lists useflags, their default status (+use, -use, …) and their description.

# FOR TRANSLATORS:
#   If you want to translate the useflags descriptions, just redefine them
#   with use_usename[2]="new description" inside a
#   `if [[ $LANG =~ locale_LOCALE.ENCODING ]]; then` and `fi` bloc.
#   An example is available at the end of this file for french. (and it is incomplete)

# General useflags
use_nls=(     1 "Use the Native Language Support install translations."                    )
use_free=(    1 "Use only Free Software and deblob blobed softwares when possible."        )
use_minimal=( 1 "Enable as few features as possible."                                      )
use_man=(     1 "Build and install man pages."                                             )
use_info=(    1 "Build and install texinfo pages."                                         )
use_binary=(  1 "Use official binaries instead of building when this option is available." )

# Free, open programming languages
use_perl=(   1 "Add Perl support or binding."   )
use_lua=(    1 "Add Lua support or binding."    )
use_cxx=(    1 "Add C++ support or binding."    )
use_python=( 1 "Add Python support or binding." )

# Still free and open, but a little less used languages.
use_vala=(   0 "Add Vala support or binding."   )
use_ruby=(   0 "Add Ruby support or binding."   )

# Not-that-free languages
use_java=(   0 "Add Java support or binding."   )
use_csharp=( 0 "Add C# support or binding."     )

if [[ "$LANG" =~ fr_FR.* ]]; then
	use_nls[2]="Utiliser la Prise en charge des Langues Natives."
	use_free[2]="Utiliser uniquement du Logiciel Libre et déblober les logiciels blobés si possible."
fi

