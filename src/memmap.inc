// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

createDataBlock(CopyrightHeader,        0xc00000, 0xc0007f)

createDataBlock(rom0,                   0xc00080, 0xc05fff)

createDataBlock(breakIdStrings,         0xc06000, 0xc067ff)

createDataBlock(RES_Lists,              0xc06800, 0xc07fff)

createCodeBlock(code,                   0xc08000, 0xc0ffaf)

createDataBlock(RES_Block0,             0xc10000, 0xc13fff)
createDataBlock(RES_Block1,             0xc14000, 0xc17fff)
createDataBlock(RES_Block2,             0xc18000, 0xc1bfff)
createDataBlock(RES_Block3,             0xc1c000, 0xc1ffff)


createRamBlock(dp,          0x000000, 0x0000ff)
createRamBlock(entityBlock, 0x7e0100, 0x7e10ff)
createRamBlock(shadow,      0x7e1100, 0x7e1f7f)
createRamBlock(stack,       0x7e1f80, 0x7e1fff)
createRamBlock(wram7e,      0x7e2000, 0x7effff)
createRamBlock(wram7f,      0x7f0000, 0x7fffff)


// Temporary Variables
// ===================
evaluate n = 0
while {n} < 20 {
    allocate(dpTmp{n}, dp, 2)
    evaluate n = {n} + 1
}


// Entity Layout
// =============
namespace Entity {
    constant ENTITY_SIZE = 64
    constant N_ENTITIES = 64
}

define PLAYER_PROJECTILE_ENTITY_LIST = PlayerProjectiles
define ENEMY_PROJECTILE_ENTITY_LIST  = EnemyProjectiles


// VRAM Layout
// ===========
constant VRAM_OBJ_WADDR = 0x6000
constant VRAM_CONSOLE_TILES_WADDR = 0x1000
constant VRAM_CONSOLE_MAP_WADDR = 0x0000

constant VRAM_STARFIELD_TILES_WADDR = 0x2000
constant VRAM_STARFIELD_MAP_WADDR = 0x0400



// MetaTile engine settings
// ========================
constant MetaTiles.MAP_SIZE = 16384

// Unused but requited by untech-engine
constant METATILES_MAP_WADDR = 0x0400   // 64x32 tiles in size
constant METATILES_TILES_WADDR = 0x2000


// vim: ft=bass-65816 ts=4 sw=4 et:

