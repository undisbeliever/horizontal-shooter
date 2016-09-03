
BINARY         := bin/horizontal-shooter.sfc
MAIN           := src/main.asm
INCLUDES       := $(wildcard src/* src/*/*.* src/*/*/*.* src/*/*/*/*.*)

RESOURCE_DIRS  := $(wildcard resources/*) tables/

.DELETE_ON_ERROR:

.PHONY: all
all: $(BINARY)

$(INCLUDES):
$(BINARY): resources bin/ $(INCLUDES)
	bass -strict -create -o $@ -sym $(@:.sfc=.sym) $(MAIN)

bin/:
	mkdir -p $@


.PHONY: resources
resources:
	$(foreach d,$(RESOURCE_DIRS), $(MAKE) -C $d;)



.PHONY: clean
clean:
	$(RM) $(BINARY)
	$(foreach d,$(RESOURCE_DIRS), $(MAKE) -C $d clean;)

