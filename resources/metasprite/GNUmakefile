
UTSI_SRC	= $(wildcard utsi/*/*.utsi)
UTMS_SRC	= $(wildcard utms/*/*.utms)
FORMATS_SRC	= $(wildcard */*/*.utft)

UTMS_GEN	= $(patsubst utsi/%.utsi,utms-gen/%.utms,$(UTSI_SRC))

.DELETE_ON_ERROR:

.PHONY: all
all: dirs metasprites.gen.inc

metasprites.gen.inc: $(UTMS_SRC) $(UTMS_GEN) $(FORMATS_SRC)

metasprites.gen.inc: metasprites.utex
	untech-msc -o "$@" "$<"

utms-gen/%.utms: utsi/%.utsi utsi/%.png
	untech-utsi2utms -o "$@" "$<"


.PHONY: dirs
GEN_DIRS := $(sort $(dir $(UTMS_GEN)))
dirs: utms-gen/ $(GEN_DIRS)
utms-gen/ $(GEN_DIRS):
	mkdir "$@"


.PHONY: clean
clean:
	$(RM) metasprites.gen.inc
	$(RM) $(UTMS_GEN)

