
UTSI_SRC	= $(wildcard utsi/*/*.utsi)
UTMS_SRC	= $(wildcard utms/*/*.utms)
FORMATS_SRC	= $(wildcard */*/*.utft)
IMAGES          = $(wildcard */*/*.png)

.DELETE_ON_ERROR:

.PHONY: all
all: metasprites.gen.inc

metasprites.gen.inc: $(UTSI_SRC) $(UTMS_GEN) $(FORMATS_SRC) $(IMAGES)

metasprites.gen.inc: metasprites.utmspro
	untech-msc -o "$@" "$<"

.PHONY: clean
clean:
	$(RM) metasprites.gen.inc

