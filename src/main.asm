arch snes.cpu

define MEMORY_MAP(HIROM)
define ROM_SIZE(1)
define ROM_SPEED(slow)
define REGION(Australia)
define ROM_NAME("HORIZONTAL SHOOTER")
define VERSION(0)

include "../untech/src/common.inc"
include "memmap.inc"

include "../resources/metasprite/metasprites.gen.inc"
include "../resources/text/text.inc"
include "../resources/images/images.inc"

include "../tables/tables.inc"
include "../untech/tables/tables.inc"

include "../untech/src/dma.inc"
include "../untech/src/hdma.inc"
include "../untech/src/interrupts.inc"
include "../untech/src/math.inc"
include "../untech/src/metasprite.inc"
include "../untech/src/text.inc"

include "../untech/src/camera.inc"
include "../untech/src/entity.inc"

include "interface.inc"
include "spawner.inc"
include "hacks.inc"
include "physics.inc"

include "gameloop.inc"

include "camera/starfield.inc"

include "entities/_base-enemy.inc"
include "entities/missiles.inc"
include "entities/player.inc"
include "entities/particles.inc"
include "entities/enemy-fighter.inc"
include "entities/enemy-sinusoidal.inc"
include "entities/enemy-carrier.inc"
include "entities/enemy-carrier-drone.inc"

code(code)
CopHandler:
IrqHandler:
EmptyHandler:
    rti


constant Main(GameLoop.Restart)

// vim: ft=bass-65816 ts=4 sw=4 et:

