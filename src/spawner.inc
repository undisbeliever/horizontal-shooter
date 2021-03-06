// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Spawner {

    // id of previous spawn table
    // (word)
    allocate(previousTableId, wram7e, 2)


// REQUIRES: 16 bit A, DB = 0x7e
macro Init() {
    assert16a()

    lda.w   #0xffff
    sta.w   Spawner.previousTableId
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
macro ProcessGameLoop() {
    assert16a()
    assert16i()

    // Spawn if player has HP and no enemies on screen

    lda.w   {Entity.listAddress(Enemies)}
    bne     EndIf

    ldy.w   GameLoop.playerEntity
    lda.w   Entities.Player.EntityStruct.hp,y
    beq     EndIf

        jsr     Spawner.Spawn
    EndIf:
}


// Spawns a group of enemies according the data table
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e, Entities setup
a16()
i16()
function Spawn {
allocateTmpWord(tmp_address)
allocateTmpWord(tmp_count)

    // Ensure enities are still valid
    jsr     Entity.ValidateEntityLists

    // ::TODO select random table not of previous

    lda.w   previousTableId
    inc
    cmp.w   #SpawnTableList.N_ELEMENTS
    bcc     +
        lda.w   #0
    +
    sta.w   previousTableId

    asl
    tax
    lda.l   SpawnTableList,x
    tax


    lda.l   SpawnTableOffset,x
    and.w   #0x0f
    beq     Return
    sta.w   tmp_count


    Loop:
        phx
            lda.l   SpawnTableRow.xPos,x
            clc
            adc.w   Camera.xPos
            sta.w   Entity.Spawn.xPos

            lda.l   SpawnTableRow.yPos,x
            clc
            adc.w   Camera.yPos
            sta.w   Entity.Spawn.yPos

            lda.l   SpawnTableRow.parameter,x
            tay
            lda.l   SpawnTableRow.entityId,x

            jsr     Entity.Spawn

        pla
        clc
        adc.w   #SpawnTableRow.size
        tax

        dec.w   tmp_count
        bne     Loop

Return:
    rts
}



rodata(rom0)
constant SpawnTableOffset = pc() & 0xff0000

namespace SpawnTableRow {
    struct(SpawnTableOffset + 1)
        field(entityId, 2)
        field(parameter,2)
        field(xPos, 2)
        field(yPos, 2)
    endstruct()
}

SpawnTableData:
namespace SpawnTableData {
    constant XSTART = Camera.DISPLAY_WIDTH + Entity.ACTIVE_WINDOW_EXTEND_HORIZONTAL / 2
    evaluate count = 0

    inline table() {
        _{count}:
        evaluate count = {count} + 1
    }

    macro row(entity, evaluate param, evaluate xPos, evaluate yPos) {
        dw  Project.EntityIds.{entity}, {param}, XSTART + {xPos}, {yPos}
    }

    table()
        db 12
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 0)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 1)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 2)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 3)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 4)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 5)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 6)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 7)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 8)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 9)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 10)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 11)

    table()
        db 3
        row(EnemySinusoidal_Slow,   0,     20,      112 - 10)
        row(EnemySinusoidal_Slow,   0,      0,      112 + 0)
        row(EnemySinusoidal_Slow,   0,     20,      112 + 10)

    table()
        db 7
        row(EnemySinusoidal_Slow,   0,     30,      112 - 30)
        row(EnemySinusoidal_Slow,   0,     20,      112 - 20)
        row(EnemySinusoidal_Slow,   0,     10,      112 - 10)
        row(EnemySinusoidal_Slow,   0,      0,      112 + 0)
        row(EnemySinusoidal_Slow,   0,     10,      112 + 10)
        row(EnemySinusoidal_Slow,   0,     20,      112 + 20)
        row(EnemySinusoidal_Slow,   0,     30,      112 + 30)

    table()
        db 12
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 0)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 1)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 2)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 3)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 4)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 5)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 6)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 7)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 8)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 9)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 10)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 11)

    table()
        db 3
        row(EnemySinusoidal_Fast,   0,     20,      112 - 10)
        row(EnemySinusoidal_Fast,   0,      0,      112 + 0)
        row(EnemySinusoidal_Fast,   0,     20,      112 + 10)

    table()
        db 6
        row(EnemySinusoidal_Slow,   0,     20,      40 - 10)
        row(EnemySinusoidal_Slow,   0,      0,      40 + 0)
        row(EnemySinusoidal_Slow,   0,     20,      40 + 10)
        row(EnemySinusoidal_Slow,   0,     20,      184 - 10)
        row(EnemySinusoidal_Slow,   0,      0,      184 + 0)
        row(EnemySinusoidal_Slow,   0,     20,      184 + 10)

    table()
        db 12
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 0)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 1)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 2)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 3)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 4)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 5)
        row(EnemyFighter_Fast,      0,      0,      18 + 17 * 6)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 7)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 8)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 9)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 10)
        row(EnemyFighter_Slow,      0,      0,      18 + 17 * 11)

    table()
        db 11
        row(EnemySinusoidal_Medium, 0,     20,      112 - 10)
        row(EnemySinusoidal_Medium, 0,      0,      112 + 0)
        row(EnemySinusoidal_Medium, 0,     20,      112 + 10)

        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 0)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 1)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 2)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 3)

        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 8)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 9)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 10)
        row(EnemyFighter_Medium,    0,      0,      18 + 17 * 11)

    table()
        db 9
        row(EnemySinusoidal_Slow,   0,     20,      184 - 10)
        row(EnemySinusoidal_Slow,   0,      0,      184 + 0)
        row(EnemySinusoidal_Slow,   0,     20,      184 + 10)

        row(EnemySinusoidal_Medium, 0,     20,      112 - 10)
        row(EnemySinusoidal_Medium, 0,      0,      112 + 0)
        row(EnemySinusoidal_Medium, 0,     20,      112 + 10)

        row(EnemySinusoidal_Slow,   0,     20,      40 - 10)
        row(EnemySinusoidal_Slow,   0,      0,      40 + 0)
        row(EnemySinusoidal_Slow,   0,     20,      40 + 10)

    table()
        db 1
        row(EnemyCarrier,           0,      0,      112)
}

SpawnTableList:
    evaluate n = 0
    while {n} < {SpawnTableData.count} {
        dw  SpawnTableData._{n}

        evaluate n = {n} + 1
    }
    constant SpawnTableList.N_ELEMENTS = (pc() - SpawnTableList) / 2


code()
}

// vim: ft=bass-65816 ts=4 sw=4 et:

