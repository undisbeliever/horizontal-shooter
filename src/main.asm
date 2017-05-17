// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

arch snes.cpu

define MEMORY_MAP(HIROM)
define ROM_SIZE(1)
define ROM_SPEED(slow)
define REGION(Australia)
define ROM_NAME("HORIZONTAL SHOOTER")
define VERSION(0)

include "../untech/src/common.inc"
include "memmap.inc"

include "resources.inc"

include "../gen/tables/sine.inc"
include "../gen/tables/entityhitbox-collisionorder.inc"

include "../untech/src/untech.inc"

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


finalizeMemory()

// vim: ft=bass-65816 ts=4 sw=4 et:

