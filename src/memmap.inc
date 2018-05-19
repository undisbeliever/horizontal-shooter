// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

architecture wdc65816

createCodeBlock(code,                   0xc08000, 0xc0ffaf)
createDataBlock(rom0,                   0xc00080, 0xc04fff)

createDataBlock(CopyrightHeader,        0xc00000, 0xc0007f)


createDataBlock(MS_FrameSetList,        0xc10000, 0xc100ff)
createDataBlock(MS_PaletteList,         0xc10100, 0xc101ff)
createDataBlock(MS_FrameList,           0xc10200, 0xc102ff)
createDataBlock(MS_AnimationList,       0xc10300, 0xc103ff)

createDataBlock(EN_EntityList,          0xc10400, 0xc1047f)
createDataBlock(EN_ProjectileList,      0xc10480, 0xc104ff)

createDataBlock(DMA_Tile16Data,         0xc10500, 0xc10fff)


createDataBlock(MS_FrameSetData,        0xc11000, 0xc11fff)
createDataBlock(MS_FrameData,           0xc12000, 0xc12fff)
createDataBlock(MS_AnimationData,       0xc13000, 0xc13fff)
createDataBlock(MS_FrameObjectsData,    0xc14000, 0xc14fff)
createDataBlock(MS_TileHitboxData,      0xc15000, 0xc15fff)
createDataBlock(MS_EntityHitboxData,    0xc16000, 0xc16fff)
createDataBlock(MS_ActionPointsData,    0xc17000, 0xc17fff)

createDataBlock(MS_PaletteData,         0xc18000, 0xc18fff)

createDataBlock(EN_RomData,             0xc19000, 0xc19fff)

createDataBlock(RES_Lists,              0xc05000, 0xc05fff)
createDataBlock(RES_Block0,             0xc06000, 0xc07fff)

createDataBlock(MS_TileBlock_0,         0xc1a000, 0xc1bfff)
createDataBlock(MS_TileBlock_1,         0xc1c000, 0xc1dfff)
createDataBlock(MS_TileBlock_2,         0xc1e000, 0xc1ffff)


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


// VRAM Layout
// ===========
constant VRAM_OBJ_WADDR = 0x6000
constant VRAM_CONSOLE_TILES_WADDR = 0x1000
constant VRAM_CONSOLE_MAP_WADDR = 0x0000

constant VRAM_STARFIELD_TILES_WADDR = 0x2000
constant VRAM_STARFIELD_MAP_WADDR = 0x0400



// MetaTile engine settings
// ========================
constant MetaTiles.MAP_SIZE = 8192
constant MetaTiles.N_META_TILES = 512

// Unused but requited by untech-engine
constant METATILES_MAP_WADDR = 0x0400   // 64x32 tiles in size
constant METATILES_TILES_WADDR = 0x2000


// vim: ft=bass-65816 ts=4 sw=4 et:

