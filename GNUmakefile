
BINARY         := bin/horizontal-shooter.sfc
MAIN           := src/main.asm

SRC_DIRS       := src untech/src resources
INCLUDES       := $(foreach d,$(SRC_DIRS),$(wildcard $d/*.inc $d/*/*.inc $d/*/*/*.inc $d/*/*/*/*.inc))


.DELETE_ON_ERROR:


.PHONY: all
all: directories $(BINARY)



.PHONY: tables
TABLE_DIRS     := tables untech/tables
TABLE_INCS     := $(foreach d,$(TABLE_DIRS),$(patsubst $d/%.py,gen/tables/%.inc,$(wildcard $d/*.py)))
tables: $(TABLE_INCS)

gen/tables/%.inc: tables/%.py
	python3 $< >| $@

gen/tables/%.inc: untech/tables/%.py
	python3 $< >| $@



.PHONY: resources
resources: metasprites font images
RESOURCES :=


.PHONY: metasprites
metasprites: gen/metasprites.inc
gen/metasprites.inc: $(wildcard resources/metasprite/*.*)
gen/metasprites.inc: $(wildcard resources/metasprite/*/*.*)
gen/metasprites.inc: $(wildcard resources/metasprite/*/*/*.*)
gen/metasprites.inc: resources/metasprite/metasprites.utmspro
	untech-msc -o "$@" "$<"

RESOURCES += gen/metasprites.inc


gen/resources.inc gen/resources.bin: resources/resources.utres
	untech-resc --output-inc gen/resources.inc --output-bin gen/resources.bin "$<"

RESOURCES += gen/resources.inc gen/resources.bin



.PHONY: font
font: gen/font-fixed.1bpp
gen/font-fixed.1bpp: resources/text/font-fixed.png
	untech-png2tileset -b 1 -o "$@" "$<"

RESOURCES += gen/font-fixed.1bpp


.PHONY: images
IMAGE_TILES    := gen/images/stars.4bpp
IMAGE_MAPS     := $(patsubst(%.4bpp,%.map,$(IMAGE_TILES))
IMAGE_PALETTES := $(patsubst(%.map,%.clr,$(IMAGE_MAPS))

images: $(IMAGE_TILES) $(IMAGE_MAPS) $(IMAGE_PALETTES)

gen/images/stars.4bpp gen/images/stars.clr gen/images/stars.map: resources/images/stars.png
	untech-png2snes -b 4 --palette-offset 1 --max-palettes 7 -t $@ -p $(@:.4bpp=.clr) -m $(@:.4bpp=.map) $<

RESOURCES += $(IMAGE_TILES) $(IMAGE_MAPS) $(IMAGE_PALETTES)



.PHONY: directories
DIRS := $(sort $(dir $(BINARY) $(RESOURCES) $(TABLE_INCS)))
directories: $(DIRS)
$(DIRS):
	mkdir -p $@



$(BINARY) $(BINARY:.sfc=.sym): $(MAIN) $(INCLUDES) $(TABLE_INCS) $(RESOURCES)
	bass-untech -strict -o $@ -sym $(@:.sfc=.sym) $(MAIN)
	untech-write-sfc-checksum --hirom $@


.PHONY: memory-usage
memory-usage: directories $(BINARY)
	bass-untech -d SHOW_MEMORY_USAGE -strict $(MAIN)



.PHONY: clean
clean:
	$(RM) $(BINARY) $(BINARY:.sfc=.sym)
	$(RM) $(sort $(TABLE_INCS))
	$(RM) $(sort $(RESOURCES))

