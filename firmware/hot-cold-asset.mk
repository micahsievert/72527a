# if "template" is in the make command, do not include static.lib files
ifneq (,$(findstring template,$(MAKECMDGOALS)))
ASSET_FILES=$(wildcard static/*)
else
ASSET_FILES=$(wildcard static/*) $(wildcard static.lib/*)
endif

# LemLib.a already contains this demo asset. Embedding it again can collide
# with its cold-package symbol, especially after Windows line-ending conversion.
# The robot uses figure8.txt; keep the unused example file out of the link.
ASSET_FILES := $(filter-out static/example.txt,$(ASSET_FILES))

TEMPLATE_FILES+=$(wildcard static/*) $(wildcard firmware/hot-cold-asset.mk)

ASSET_OBJ=$(addprefix $(BINDIR)/, $(addsuffix .o, $(ASSET_FILES)) )

GETALLOBJ=$(sort $(call ASMOBJ,$1) $(call COBJ,$1) $(call CXXOBJ,$1)) $(ASSET_OBJ)

.SECONDEXPANSION:
$(ASSET_OBJ): $$(patsubst bin/%,%,$$(basename $$@))
	$(VV)mkdir -p $(BINDIR)/static
	$(VV)mkdir -p $(BINDIR)/static.lib
	@echo "ASSET $@"
	$(VV)$(OBJCOPY) -I binary -O elf32-littlearm -B arm $^ $@
