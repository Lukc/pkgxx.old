
#define __FP_SED sed \
-e "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" \
-e "s|	link	to	| -> |" \
-e "s|	->	| -> |"

#define __FP_GTAR(__TARGET) tar tvvf __TARGET | \
sed 's|  *|	|g' | \
cut -d "	" -f 1,2,6,7,8

#define __FP_BSDTAR(__TARGET) bsdtar tvf __TARGET | \
sed 's|  *|	|g' | \
cut -d "	" -f 1,3,4,9,10,11,12

