
.DELETE_ON_ERROR:

.PHONY: all
all: stars.4bpp stars.clr stars.map

stars.4bpp stars.clr stars.map: stars.png
	untech-png2snes -b 4 --palette-offset 1 --max-palettes 7 -t stars.4bpp -p stars.clr -m stars.map stars.png

.PHONY: clean
clean:
	$(RM) stars.4bpp stars.clr stars.map

