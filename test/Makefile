
ok = \033[32m ok \033[00m
fail = \033[31mfail\033[00m

PKGXX ?= pkg++

nano: check_root
	@echo " [ Cleaning                                   ]"
	@$(PKGXX) -cf test-config nano.Pkgfile -c &> log # Remove any unwanted existing file.
	@echo " [ Breaking footprint, sha256sum and md5sum   ]"
	@echo "xxx" >> .footprint # We make sure the footprint, sha256 and md5 sums are br   oken.
	@echo "yyy" >> .sha256sum
	@echo "zzz" >> .md5sum
	@echo " [ Building test package                      ]"
	@! $(PKGXX) -cf test-config nano.Pkgfile -d -dg -is -im &>> log # Will (should) fail.
	@echo " [ Building footprint                         ]"
	@$(PKGXX) -cf test-config nano.Pkgfile -uf &>> log # Shouldn’t fail.
	@echo " [ Building control sum                       ]"
	@$(PKGXX) -cf test-config nano.Pkgfile -us -um &>> log # .* clean again.
	@if [ -e nano-2.2.6.tar.gz ]; then \
		echo -e " > File download...                       $(ok)" ; \
	else \
		echo -e " > File download...                       $(fail) " ; \
	fi
	@if [ -e nano-man\#2.2.6-1-*.pkg.tar.gz ]; then \
		echo -e " > Packages splits...                     $(ok)" ; \
	else \
		echo -e " > Packages splits...                     $(fail)" ; \
	fi
	@if [ -e nano-man\#2.2.6-1-noarch.pkg.tar.gz ]; then \
		echo -e " > Packages splits noarch...              $(ok)" ; \
	else \
		echo -e " > Packages splits noarch...              $(fail)" ; \
	fi
	@if [ -n "`cat .footprint`" ]; then \
		echo -e " > Footprint creation...                  $(ok)" ; \
	else \
		echo -e " > Footprint creation...                  $(fail)" ; \
	fi
	@$(PKGXX) -cf test-config nano.Pkgfile -c &>> log # Remove any unwanted existing file again.
	@if ! [ -e nano-man\#2.2.6-1-`uname -m`.pkg.tar.gz ]; then \
		echo -e " > Splits cleaning...                     $(ok)" ; \
	else \
		echo -e " > Splits cleaning...                     $(fail)" ; \
	fi
	@if ! [ -d nano-man ] && ! [ -d nano-info ]; then \
		echo -e " > Splits clean dirs...                   $(ok)" ; \
	else \
		echo -e " > Splits clean dirs...                   $(fail)" ; \
		rmdir */ || true ; \
	fi
	@if [ -e nano-man*.pkg.tar.gz ] && ! [ -e nano-info*.pkg.tar.gz ]; then \
		echo -e " > No empty splits...                     $(ok)" ; \
	else \
		echo -e " > No empty splits...                     $(fail)" ; \
	fi

check_root:
	@if [ $$UID != 0 ]; then \
		echo -e "  --- > You must run this test as root." ; \
		exit 1 ; \
	fi

