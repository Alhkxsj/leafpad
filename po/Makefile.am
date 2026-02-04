linguas = be bg br ca cs da de el eo es eu fi fr fur ga gl he hr hu id it ja ko ky lt lv nb nl nn pl pt pt_BR ru sk sl sq sr sv ta th tr uk vi zh_CN zh_TW
.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -c -o $$file $<
.po.pox:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.pox \
	  && rm -f $$file && $(MSGMERGE) --prefix=$(MSGMERGE_PREFIX) $< $(srcdir)/$(GETTEXT_PACKAGE).pot -o $$file
$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(GETTEXT_PACKAGE) \
	  --directory=$(top_srcdir) \
	  --add-comments=TRANSLATORS: \
	  --keyword=_ --keyword=N_ \
	  --msgid-bugs-address=$(GETTEXT_PACKAGE_BUGREPORT) \
	  --files-from=$(srcdir)/POTFILES.in \
	  --copyright-holder=$(COPYRIGHT_HOLDER) \
	  --from-code=CHARSET \
	  && test ! -f $(GETTEXT_PACKAGE).po \
	  || ( rm -f $(GETTEXT_PACKAGE).pot \
	  && mv $(GETTEXT_PACKAGE).po $(GETTEXT_PACKAGE).pot )
install-data-local:
	@$(NORMAL_INSTALL)
	@for lang in $(linguas); do \
	  dir=$(DESTDIR)$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkdir_p) $$dir; \
	  if test -r $(srcdir)/$$lang.gmo; then \
	    $(INSTALL_DATA) $(srcdir)/$$lang.gmo $$dir/$(GETTEXT_PACKAGE).mo; \
	  elif test -r $(srcdir)/$$lang.po; then \
	    $(GMSGFMT) -c -o $$dir/$(GETTEXT_PACKAGE).mo $(srcdir)/$$lang.po; \
	  else \
	    echo "Skipping language $$lang: no .po or .gmo file found"; \
	  fi; \
	done
uninstall-local:
	@for lang in $(linguas); do \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE).mo; \
	done
CLEANFILES = *.gmo POTFILES
EXTRA_DIST = $(POFILES) $(LINGUAS) POTFILES.in
MAINTAINERCLEANFILES = $(GMOFILES) $(GETTEXT_PACKAGE).pot

# Override clean target to remove Makefile
clean-local:
	-rm -f Makefile