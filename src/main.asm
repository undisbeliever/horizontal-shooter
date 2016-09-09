arch snes.cpu

define MEMORY_MAP(HIROM)
define ROM_SIZE(1)
define ROM_SPEED(slow)
define REGION(Australia)
define ROM_NAME("HORIZONTAL SHOOTER")
define VERSION(0)

include "untech/common.inc"
include "memmap.inc"

include "../resources/metasprite/metasprites.gen.inc"
include "../resources/text/text.inc"
include "../resources/images/images.inc"

include "../tables/tables.inc"
include "../tables2/tables.inc"

include "untech/dma.inc"
include "untech/interrupts.inc"
include "untech/math.inc"
include "untech/metasprite.inc"
include "untech/text.inc"

include "untech/camera.inc"
include "untech/entity.inc"


include "spawner.inc"
include "hacks.inc"
include "gameloop.inc"
include "physics.inc"

include "camera/starfield.inc"

include "entities/_base-enemy.inc"
include "entities/missiles.inc"
include "entities/player.inc"
include "entities/enemy-fighter.inc"
include "entities/enemy-sinusoidal.inc"

code(code)
CopHandler:
IrqHandler:
EmptyHandler:
    rti


code(code)
Main:
    rep     #$30
a16()
i16()
    jsr     GameLoop.Init
    jmp     GameLoop.Run

// vim: ft=asm ts=4 sw=4 et:

