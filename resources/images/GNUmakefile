
.DELETE_ON_ERROR:

.PHONY: all
all: stars.4bpp stars.clr stars.map

stars.4bpp stars.clr stars.map: stars.pcx
	pcx2snes -n -s8 -c16 -o16 -screen stars
	mv stars.pic stars.4bpp
	python3 palette-offset-map.py stars.map 1

.PHONY: clean
clean:
	$(RM) stars.4bpp stars.clr stars.map

